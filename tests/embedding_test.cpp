#include <embedding.h>

#include <cassert>
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

    return 0;
}
