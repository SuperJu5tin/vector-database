/* Chunking Component Class
 * Author: Justin Libhart
 * Date: 2026-11-16
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: File Handler Class for Reading and Writing Data to Files. Every chunk is a paragraph. The chunks are stored in chapters
 */

#include "chunking_component.h"
#include <atomic>
#include <regex>
#include <stdexcept>
#include <utility>

namespace {
std::atomic<int> nextChunkId{1};
}

Chunk::Chunk(std::string text)
    : text(std::move(text)),
      id(nextChunkId.fetch_add(1, std::memory_order_relaxed)) {}

int Chunk::getId() const {
    return id;
}

const std::string& Chunk::getText() const {
    return text;
}

Chapter::Chapter(std::string bodyText, int number)
    : number(number), chunks(splitChunks(bodyText)) {}

void Chapter::setNumber(int number) {
    this->number = number;
}

void Chapter::setChunks(std::vector<Chunk> chunks) {
    this->chunks = std::move(chunks);
}

int Chapter::getNumber() const {
    return number;
}

std::vector<Chunk> Chapter::getChunks() const {
    return chunks;
}

std::vector<Chunk> Chapter::splitChunks(std::string chapter) {
    const std::regex paragraphBreak(R"((?:\r?\n[ \t]*)+\r?\n)");
    std::vector<Chunk> splitChunks;

    std::size_t start = 0;
    std::sregex_iterator current(chapter.begin(), chapter.end(), paragraphBreak);
    const std::sregex_iterator end;

    const auto addParagraph = [&](std::size_t paragraphEnd) {
        const std::string paragraph = chapter.substr(start, paragraphEnd - start);
        if (paragraph.find_first_not_of(" \t\r\n") != std::string::npos) {
            splitChunks.emplace_back(paragraph);
        }
    };

    for (; current != end; ++current) {
        const std::size_t breakPoint = static_cast<std::size_t>(current->position());
        addParagraph(breakPoint);
        start = breakPoint + static_cast<std::size_t>(current->length());
    }

    addParagraph(chapter.length());
    return splitChunks;
}

ChunkedComponent::ChunkedComponent(std::string chunkName, std::string data) {
    this->chunkName = chunkName;

    std::vector<std::string> chapters = splitChapters(data);

    // group chapters into data structure
    std::vector<Chapter> chaptersTyped;
    int index = 1;
    for (const std::string& chapterBody : chapters) {
        Chapter chapter = Chapter(chapterBody, index);
        chaptersTyped.push_back(chapter);
        index++;
    }

    this->chapters = chaptersTyped;
}

void ChunkedComponent::setChapters(std::vector<Chapter> chapters) {
    this->chapters = std::move(chapters);
}

std::vector<std::string> ChunkedComponent::getChunks() const {
    std::vector<std::string> chunkTexts;
    for (const Chapter& chapter : chapters) {
        for (const Chunk& chunk : chapter.getChunks()) {
            chunkTexts.push_back(chunk.getText());
        }
    }
    return chunkTexts;
}

std::vector<int> ChunkedComponent::getChunkIds() const {
    std::vector<int> chunkIds;
    for (const Chapter& chapter : chapters) {
        for (const Chunk& chunk : chapter.getChunks()) {
            chunkIds.push_back(chunk.getId());
        }
    }
    return chunkIds;
}

Chapter ChunkedComponent::getChapterFromChunk(int chunkId) const {
    for (const Chapter& chapter : chapters) {
        for (const Chunk& chunk : chapter.getChunks()) {
            if (chunk.getId() == chunkId) {
                return chapter;
            }
        }
    }

    throw std::out_of_range("No chapter contains the requested chunk ID");
}

std::vector<std::string> ChunkedComponent::splitChapters(std::string bookData) {
    const std::regex chapterBreakPoint(R"(Chapter [0-9]+)");
    std::vector<std::string> splitChapters;

    std::size_t start = 0;
    std::sregex_iterator current(bookData.begin(), bookData.end(), chapterBreakPoint);
    const std::sregex_iterator end;

    if (current == end) {
        splitChapters.push_back(bookData);
    } else {
        for (; current != end; ++current) {
            const std::size_t breakPoint = static_cast<std::size_t>(current->position());

            if (breakPoint > start) {
                splitChapters.push_back(bookData.substr(start, breakPoint - start));
            }

            start = breakPoint + static_cast<std::size_t>(current->length());
        }

        if (start < bookData.length()) {
            splitChapters.push_back(bookData.substr(start));
        }
    }

    return splitChapters;
}
