#pragma once

#include <string>

class Embeddings;

class FileOutput {
public:
    void writeEmbeddings(const Embeddings& embeddings,
                         const std::string& filePath) const;
};
