/* Chunking Component Class
 * Author: Justin Libhart
 * Date: 2026-11-16
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: File Handler Class for Reading and Writing Data to Files. Every chunk is a paragraph. The chunks are stored in chapters
 */

#include "chunking_component.h"

ChunkedComponent::ChunkedComponent(std::string chunkName, std::string data) {
    this->chunkName = chunkName;

    // Find all chapter breaking points
    // Book denotes this by "Chapter <number>"