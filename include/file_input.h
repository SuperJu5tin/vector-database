/* File Handler Class
 * Author: Justin Libhart
 * Date: 2026-11-16
 * Instructor: Professor Cook
 * Course: CSC-1060
 * Description: File Handler Class for Reading and Writing Data to Files
 */

#pragma once

#include <fstream>
#include <string>
#include <stdexcept>

class FileHandler {
  private:
    std::string filename;
    std::string data;

  public:
    FileHandler(std::string filename);
    std::string getFileName() const;
    std::string getData() const;
};