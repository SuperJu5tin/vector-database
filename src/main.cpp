/* Vector Database Search
 * Author: Justin Libhart
 * Date: 2026-11-09
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: Vector Database Search Engine
 */

#include "chunking_component.h"
#include "file_input.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

int main() {
    try {
        FileHandler bookFile("documents/frankenstein.txt");
        ChunkedComponent book("Frankenstein", bookFile.getData());
        const std::vector<std::string> chunks = book.getChunks();
        const std::vector<int> chunkIds = book.getChunkIds();
        std::ofstream outputFile("frankenstein_chunks.txt", std::ios::trunc);
        if (!outputFile) {
            throw std::runtime_error(
                "Unable to open frankenstein_chunks.txt for writing");
        }

        for (std::size_t index = 0; index < chunks.size(); ++index) {
            outputFile << "Chunk ID: " << chunkIds[index] << '\n'
                       << chunks[index] << "\n\n";
        }

        outputFile.close();
        if (!outputFile) {
            throw std::runtime_error(
                "Unable to write all chunks to frankenstein_chunks.txt");
        }
    } catch (const std::exception& error) {
        std::cerr << "Unable to chunk Frankenstein: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
