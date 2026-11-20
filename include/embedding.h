/* Embedding Component Class
 * Author: Justin Libhart
 * Date: 2026-11-18
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: Provides embedding storage and Ollama model integration. The Ollama class interfaces with the Nomic embedding model, while the Embedding class stores embeddings and calls the Ollama class.
 * The embedding Storage will be the vector database of sorts
 */

#pragma once
#include <vector>
#include <string>
#include <initializer_list>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <utility>

class ChunkedComponent;

class Ollama {
private:
    std::string modelURL;
    std::string modelName;
public:
    Ollama();
    std::vector<double> getEmbedding(const std::string& input);
    std::vector<std::vector<double>> getEmbeddings(
        const std::vector<std::string>& input);
};

class Embedding {
private:
    std::optional<int> id;
    std::vector<double> embedding;
public:
    explicit Embedding(std::vector<double> embedding);
    Embedding(std::vector<double> embedding, int id);
    Embedding(const std::string& text, int id);
    std::vector<double> getEmbedding() const;
    std::optional<int> getId() const;
};

class Embeddings {
private:
    std::vector<Embedding> embeddings;
    Ollama ollama;
public:
    Embeddings();
    Embeddings(std::vector<double> embedding);
    Embeddings(std::vector<std::vector<double>> embeddings);
    Embeddings(std::initializer_list<double> embedding);
    Embeddings(std::initializer_list<std::initializer_list<double>> embeddings);
    Embeddings(const std::string& input);
    Embeddings(const std::vector<std::string>& input);
    Embeddings(const ChunkedComponent& component);
    Embeddings(const std::string& bookName, const std::string& filePath);
    void addEmbedding(std::vector<double> embedding);
    void addEmbeddings(std::vector<std::vector<double>> embeddings);
    void addEmbedding(std::initializer_list<double> embedding);
    void addEmbeddings(std::initializer_list<std::initializer_list<double>> embeddings);
    void addEmbedding(const std::string& input);
    void addEmbeddings(const std::vector<std::string>& input);
    float cosineSimilarityCalc(Embedding A, Embedding B);
    std::vector<std::vector<double>> getEmbeddings() const;
    std::vector<Embedding> getEmbeddingObjects() const;
};