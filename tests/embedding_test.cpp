#include <embedding.h>
#include <chunking_component.h>
#include <file_output.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

int main() {
    Ollama ollama;
    const std::vector<std::vector<double>> embeddings =
        ollama.getEmbeddings({});

    assert(embeddings.empty());

    Embeddings empty;
    assert(empty.getEmbeddings().empty());

    const std::vector<double> first = {0.1, 0.2};
    Embeddings single(first);
    assert(single.getEmbeddings() == std::vector<std::vector<double>>{first});

    const std::vector<std::vector<double>> initial = {{0.3, 0.4}, {0.5, 0.6}};
    Embeddings multiple(initial);
    assert(multiple.getEmbeddings() == initial);

    multiple.addEmbedding({0.7, 0.8});
    multiple.addEmbeddings({{0.9, 1.0}, {1.1, 1.2}});
    assert((multiple.getEmbeddings() ==
            std::vector<std::vector<double>>{
                {0.3, 0.4}, {0.5, 0.6}, {0.7, 0.8},
                {0.9, 1.0}, {1.1, 1.2}}));

    Embedding precomputed(std::vector<double>{0.1, 0.2, 0.3}, 7);
    assert(precomputed.getId() == 7);
    assert((precomputed.getEmbedding() == std::vector<double>{0.1, 0.2, 0.3}));

    Embedding generated("cat", 8);
    assert(generated.getId() == 8);
    assert(!generated.getEmbedding().empty());

    Embeddings fromText("cat");
    assert(fromText.getEmbeddings().size() == 1);
    assert(!fromText.getEmbeddings().front().empty());

    Embeddings fromTexts(std::vector<std::string>{"cat", "dog"});
    assert(fromTexts.getEmbeddings().size() == 2);
    assert(!fromTexts.getEmbeddings()[0].empty());
    assert(!fromTexts.getEmbeddings()[1].empty());

    Embeddings appendedText;
    appendedText.addEmbedding("cat");
    assert(appendedText.getEmbeddings().size() == 1);
    assert(!appendedText.getEmbeddings().front().empty());

    appendedText.addEmbeddings(std::vector<std::string>{"dog", "bird"});
    assert(appendedText.getEmbeddings().size() == 3);
    assert(!appendedText.getEmbeddings()[1].empty());
    assert(!appendedText.getEmbeddings()[2].empty());

    ChunkedComponent book(
        "test book", "First paragraph.\n\nSecond paragraph.");
    const std::vector<int> chunkIds = book.getChunkIds();
    Embeddings bookEmbeddings(book);
    const std::vector<Embedding> chunkEmbeddings =
        bookEmbeddings.getEmbeddingObjects();
    assert(chunkEmbeddings.size() == chunkIds.size());
    assert(bookEmbeddings.getEmbeddings().size() == chunkIds.size());
    for (std::size_t index = 0; index < chunkIds.size(); ++index) {
        assert(chunkEmbeddings[index].getId() == chunkIds[index]);
        assert(!chunkEmbeddings[index].getEmbedding().empty());
    }

    const std::filesystem::path bookPath =
        std::filesystem::temp_directory_path() / "embedding_test_book.txt";
    {
        std::ofstream bookFile(bookPath);
        assert(bookFile);
        bookFile << "Chapter 1\nFirst paragraph.\n\nSecond paragraph.\n";
    }

    Embeddings fromBookFile("test book", bookPath.string());
    const std::vector<Embedding> fileEmbeddings =
        fromBookFile.getEmbeddingObjects();
    assert(fileEmbeddings.size() == 2);
    for (const Embedding& embedding : fileEmbeddings) {
        assert(embedding.getId().has_value());
        assert(!embedding.getEmbedding().empty());
    }

    const std::filesystem::path embeddingsPath =
        bookPath.string() + ".embeddings";
    FileOutput fileOutput;
    fileOutput.writeEmbeddings(fromBookFile, embeddingsPath.string());
    std::ifstream savedEmbeddingsFile(embeddingsPath);
    const std::string savedEmbeddings{
        std::istreambuf_iterator<char>(savedEmbeddingsFile),
        std::istreambuf_iterator<char>()};
    assert(savedEmbeddings.find("Chunk ID: " +
                               std::to_string(*fileEmbeddings[0].getId())) !=
           std::string::npos);
    assert(savedEmbeddings.find('[') != std::string::npos);
    assert(savedEmbeddings.find("First paragraph.") == std::string::npos);
    std::filesystem::remove(embeddingsPath);
    std::filesystem::remove(bookPath);

    return 0;
}
