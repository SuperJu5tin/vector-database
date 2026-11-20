/* Embedding Component Class
 * Author: Justin Libhart
 * Date: 2026-11-18
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: Provides embedding storage and Ollama model integration. The Ollama class interfaces with the Nomic embedding model, while the Embedding class stores embeddings and calls the Ollama class.
 */

#include <embedding.h>
#include <chunking_component.h>
#include <file_input.h>

using json = nlohmann::json;

namespace {
ChunkedComponent loadBook(const std::string& bookName,
                          const std::string& filePath) {
    FileHandler file(filePath);
    return ChunkedComponent(bookName, file.getData());
}

size_t appendResponse(char* data, size_t size, size_t count, void* context) {
    const size_t totalSize = size * count;
    std::string* response = static_cast<std::string*>(context);
    response->append(data, totalSize);
    return totalSize;
}
}

Ollama::Ollama()
    : modelURL("http://localhost:11434/api/embed"),
      modelName("nomic-embed-text") {}

std::vector<double> Ollama::getEmbedding(const std::string& input) {
    const std::vector<std::vector<double>> embeddings =
        getEmbeddings(std::vector<std::string>{input});

    if (embeddings.empty()) {
        return {};
    }
    return embeddings.front();
}

// Get embeddings function to get embeddings from the ollama nomic agent
std::vector<std::vector<double>> Ollama::getEmbeddings(
    const std::vector<std::string>& input) {
    if (input.empty()) {
        return {};
    }

    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        throw std::runtime_error("Unable to initialize libcurl");
    }

    const json request = {
        {"model", modelName},
        {"input", input},
    };
    const std::string requestBody = request.dump();
    std::string responseBody;

    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, modelURL.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    const CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        const std::string error = curl_easy_strerror(result);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error("Ollama embedding request failed: " + error);
    }

    long statusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (statusCode < 200 || statusCode >= 300) {
        throw std::runtime_error(
            "Ollama embedding request returned HTTP status " +
            std::to_string(statusCode));
    }

    const json response = json::parse(responseBody);
    if (!response.contains("embeddings") ||
        !response["embeddings"].is_array()) {
        throw std::runtime_error(
            "Ollama embedding response does not contain an embeddings array");
    }

    std::vector<std::vector<double>> embeddings;
    for (const json& embedding : response["embeddings"]) {
        if (!embedding.is_array()) {
            throw std::runtime_error(
                "Ollama embedding response contains a non-array embedding");
        }
        embeddings.push_back(embedding.get<std::vector<double>>());
    }
    return embeddings;
}

Embedding::Embedding(std::vector<double> embedding)
    : embedding(std::move(embedding)) {}

Embedding::Embedding(std::vector<double> embedding, int id)
    : id(id), embedding(std::move(embedding)) {}

Embedding::Embedding(const std::string& text, int id)
    : id(id), embedding(Ollama().getEmbedding(text)) {}

std::vector<double> Embedding::getEmbedding() const {
    return embedding;
}

std::optional<int> Embedding::getId() const {
    return id;
}

Embeddings::Embeddings() = default;

Embeddings::Embeddings(std::vector<double> embedding)
    : embeddings{Embedding(std::move(embedding))} {}

Embeddings::Embeddings(std::vector<std::vector<double>> initialEmbeddings) {
    embeddings.reserve(initialEmbeddings.size());
    for (std::vector<double>& embedding : initialEmbeddings) {
        embeddings.emplace_back(std::move(embedding));
    }
}

Embeddings::Embeddings(std::initializer_list<double> embedding)
    : embeddings{Embedding(std::vector<double>(embedding))} {}

Embeddings::Embeddings(
    std::initializer_list<std::initializer_list<double>> initialEmbeddings) {
    embeddings.reserve(initialEmbeddings.size());
    for (const std::initializer_list<double>& embedding : initialEmbeddings) {
        embeddings.emplace_back(std::vector<double>(embedding));
    }
}

Embeddings::Embeddings(const std::string& input) {
    embeddings.emplace_back(ollama.getEmbedding(input));
}

Embeddings::Embeddings(const std::vector<std::string>& input) {
    std::vector<std::vector<double>> generated = ollama.getEmbeddings(input);
    embeddings.reserve(generated.size());
    for (std::vector<double>& embedding : generated) {
        embeddings.emplace_back(std::move(embedding));
    }
}

Embeddings::Embeddings(const ChunkedComponent& component) {
    const std::vector<std::string> chunks = component.getChunks();
    const std::vector<int> chunkIds = component.getChunkIds();
    if (chunks.size() != chunkIds.size()) {
        throw std::runtime_error(
            "Chunk text and ID counts do not match");
    }

    std::vector<std::vector<double>> generated = ollama.getEmbeddings(chunks);
    if (generated.size() != chunkIds.size()) {
        throw std::runtime_error(
            "Ollama returned a different number of embeddings than chunks");
    }

    embeddings.reserve(generated.size());
    for (std::size_t index = 0; index < generated.size(); ++index) {
        embeddings.emplace_back(std::move(generated[index]), chunkIds[index]);
    }
}

Embeddings::Embeddings(const std::string& bookName, const std::string& filePath)
    : Embeddings(loadBook(bookName, filePath)) {}

void Embeddings::addEmbedding(std::vector<double> embedding) {
    embeddings.emplace_back(std::move(embedding));
}

void Embeddings::addEmbeddings(
    std::vector<std::vector<double>> newEmbeddings) {
    for (std::vector<double>& embedding : newEmbeddings) {
        embeddings.emplace_back(std::move(embedding));
    }
}

void Embeddings::addEmbedding(std::initializer_list<double> embedding) {
    embeddings.emplace_back(std::vector<double>(embedding));
}

void Embeddings::addEmbeddings(
    std::initializer_list<std::initializer_list<double>> newEmbeddings) {
    embeddings.reserve(embeddings.size() + newEmbeddings.size());
    for (const std::initializer_list<double>& embedding : newEmbeddings) {
        embeddings.emplace_back(std::vector<double>(embedding));
    }
}

void Embeddings::addEmbedding(const std::string& input) {
    embeddings.emplace_back(ollama.getEmbedding(input));
}

void Embeddings::addEmbeddings(const std::vector<std::string>& input) {
    std::vector<std::vector<double>> newEmbeddings = ollama.getEmbeddings(input);
    for (std::vector<double>& embedding : newEmbeddings) {
        embeddings.emplace_back(std::move(embedding));
    }
}

std::vector<std::vector<double>> Embeddings::getEmbeddings() const {
    std::vector<std::vector<double>> result;
    result.reserve(embeddings.size());
    for (const Embedding& embedding : embeddings) {
        result.push_back(embedding.getEmbedding());
    }
    return result;
}

std::vector<Embedding> Embeddings::getEmbeddingObjects() const {
    return embeddings;
}

// Cosine Similiarity Calculation (because of modern embeddings magnitude always evaluating to 1 this is equivalent to the dot product) 
float Embeddings::cosineSimilarityCalc(Embedding A, Embedding B) {
    
    const std::vector<double>& embeddingA = A.getEmbedding();
    const std::vector<double>& embeddingB = B.getEmbedding();

    if (embeddingA.size() != embeddingB.size()) {
        throw std::invalid_argument("Embeddings must be the same size");
    }

    double dotProduct = 0.0;
    double magnitudeA = 0.0;
    double magnitudeB = 0.0;

    for (size_t i = 0; i < embeddingA.size(); ++i) {
        dotProduct += embeddingA[i] * embeddingB[i];
        magnitudeA += embeddingA[i] * embeddingA[i];
        magnitudeB += embeddingB[i] * embeddingB[i];
    }

    if (magnitudeA == 0.0 || magnitudeB == 0.0) {
        throw std::invalid_argument("Embeddings must have a non-zero magnitude");
    }

    return static_cast<float>(
        dotProduct / (std::sqrt(magnitudeA) * std::sqrt(magnitudeB)));
}

std::vector<SearchResult> Embeddings::compareTextToVDB(std::string input) {
    std::vector<SearchResult> results;

    // get embedding to compare
    Embedding searchTerm(ollama.getEmbedding(input));

    const std::vector<Embedding> listOfEmbeddings = this->getEmbeddingObjects();
    std::vector<SearchResult> listOfSearchResults;

    for (const auto& embedding : listOfEmbeddings) {
        float similarity = this->cosineSimilarityCalc(searchTerm, embedding);
        SearchResult searchResult;
        searchResult.id = embedding.getId().value_or(-1);
        searchResult.CSScore = similarity;
        results.push_back(searchResult);
    }

    return results;
};