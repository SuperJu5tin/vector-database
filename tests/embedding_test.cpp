#include <embedding.h>

#include <cassert>
#include <vector>

int main() {
    Ollama ollama;
    const std::vector<std::vector<double>> embeddings =
        ollama.getEmbeddings({});

    assert(embeddings.empty());
    return 0;
}
