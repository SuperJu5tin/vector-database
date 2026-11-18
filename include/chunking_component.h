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
public:
  explicit Chunk(std::string text);
  int getId() const;
  const std::string& getText() const;
};

class Chapter {
protected:
  int number;
  std::vector<Chunk> chunks; // paragraphs for this
public:
  Chapter(std::string bodyText, int number);
  void setNumber(int);
  void setChunks(std::vector<Chunk>);
  int getNumber() const;
  std::vector<Chunk> getChunks() const;
  std::vector<Chunk> splitChunks(std::string chapter);
};

class ChunkedComponent {
private:
  std::string chunkName;
  std::vector<Chapter> chapters;
  std::vector<std::string> splitChapters(std::string bookData);
public:
  ChunkedComponent(std::string chunkName, std::string data);
  std::vector<std::string> getChunks() const;
  std::vector<int> getChunkIds() const;
  Chapter getChapterFromChunk(int chunkId) const;
  void setChapters(std::vector<Chapter>);
};