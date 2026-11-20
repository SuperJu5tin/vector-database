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
#include <curl/curl.h>
#include <nlohmann/json.hpp>

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
    int id;
    std::vector<double> embedding;
public:
    Embedding(std::vector<double> embedding, int id);
    Embedding(const std::string& text, int id);
    std::vector<double> getEmbedding();
    int getId();
};

class Embeddings {
private:
    std::vector<std::vector<double>> embeddings;
    Ollama ollama;
public:
    Embeddings();
    Embeddings(std::vector<double> embedding);
    Embeddings(std::vector<std::vector<double>> embeddings);
    void addEmbedding(std::vector<double> embedding);
    void addEmbeddings(std::vector<std::vector<double>> embeddings);
    std::vector<std::vector<double>> getEmbeddings();
};