/* File Handler Class
 * Author: Justin Libhart
 * Date: 2026-11-16
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: File Handler Class for Reading and Writing Data to Files
 */

#include "file_input.h"
#include <iterator>

FileHandler::FileHandler(std::string filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename);
  }

  this->filename = filename;
  this->data.assign(std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>());
}

std::string FileHandler::getFileName() const {
  return filename;
}

std::string FileHandler::getData() const {
  return data;
}