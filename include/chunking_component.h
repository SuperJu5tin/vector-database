/* Chunking Component Class
 * Author: Justin Libhart
 * Date: 2026-11-16
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: File Handler Class for Reading and Writing Data to Files
 */

#pragma once

#include <string>
#include <vector>

class Chunk {
private:
  std::string text;
  int id;
};

class Chapter {
protected:
  std::string title;
  int number;
  std::vector<Chunk> chunks; // paragraphs for this
};

class ChunkedComponent {
private:
  std::string chunkName;
  std::vector<Chapter> chapters;
public:
  ChunkedComponent(std::string chunkName, std::string data);
  std::vector<std::string> getChunks() const;
  Chapter getChapterFromChunk(std::string chunk);
};