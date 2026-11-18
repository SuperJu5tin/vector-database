/* Embedding Component Class
 * Author: Justin Libhart
 * Date: 2026-11-18
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: Provides embedding storage and Ollama model integration. The Ollama class interfaces with the Nomic embedding model, while the Embedding class stores embeddings and calls the Ollama class.
 */

#include <embedding.h>

#include <stdexcept>

using json = nlohmann::json;

namespace {
size_t appendResponse(char* data, size_t size, size_t count, void* context) {
    const size_t totalSize = size * count;
    std::string* response = static_cast<std::string*>(context);
    response->append(data, totalSize);
    return totalSize;
}
}

Ollama::Ollama()
    : modelURL("http://localhost:11434/api/embed"),
      modelName("nomic-embed-text:v1.5") {}

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
