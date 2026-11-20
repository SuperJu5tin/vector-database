/* Vector Database Search
 * Author: Justin Libhart
 * Date: 2026-11-09
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: Vector Database Search Engine
 */

#include "embedding.h"
#include "file_output.h"

#include <exception>
#include <iostream>

int main() {
    try {
        const Embeddings bookEmbeddings(
            "Frankenstein", "documents/frankenstein.txt");
        FileOutput().writeEmbeddings(
            bookEmbeddings, "output/frankenstein_embeddings.txt");
    } catch (const std::exception& error) {
        std::cerr << "Unable to process Frankenstein chunks and embeddings: "
                  << error.what() << '\n';
        return 1;
    }

    return 0;
}
