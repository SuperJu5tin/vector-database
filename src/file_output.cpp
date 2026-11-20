#include "file_output.h"

#include "embedding.h"

#include <fstream>
#include <stdexcept>
#include <vector>

void FileOutput::writeEmbeddings(
    const Embeddings& embeddings, const std::string& filePath) const {
    std::ofstream outputFile(filePath, std::ios::trunc);
    if (!outputFile) {
        throw std::runtime_error("Unable to open embeddings file for writing: " +
                                 filePath);
    }

    for (const Embedding& embedding : embeddings.getEmbeddingObjects()) {
        const auto chunkId = embedding.getId();
        if (!chunkId) {
            throw std::runtime_error("Embedding is missing its chunk ID");
        }

        outputFile << "Chunk ID: " << *chunkId << '\n' << '[';
        const std::vector<double> values = embedding.getEmbedding();
        for (std::size_t index = 0; index < values.size(); ++index) {
            if (index > 0) {
                outputFile << ", ";
            }
            outputFile << values[index];
        }
        outputFile << "]\n\n";
    }

    outputFile.close();
    if (!outputFile) {
        throw std::runtime_error("Unable to write embeddings file: " + filePath);
    }
}
