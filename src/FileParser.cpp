#include "FileParser.hpp"

FileParser::FileParser(const std::string& path)
    : filePath(path) {
    openFile(path);
}

FileParser::~FileParser() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void FileParser::openFile(const std::string& filePath) {
    fileStream.open(filePath, std::ios::in); 
    
    if (!fileStream.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    fileStream.clear();
}
bool FileParser::hasMoreChars() {
    return fileStream.peek() != EOF;
}
void FileParser::reset() {
    fileStream.clear(); 
    fileStream.seekg(0, std::ios::beg); 
}

char FileParser::peekChar() {
    if (!hasMoreChars()) {
        throw std::out_of_range("End of file reached. Cannot peek.");
    }

    return static_cast<char>(fileStream.peek());
}

char FileParser::advanceChar() {
    if (!hasMoreChars()) {
        throw std::out_of_range("End of file reached. Cannot advance.");
    }
    char nextChar = static_cast<char>(fileStream.get());
    if (fileStream.fail() && !fileStream.eof()) {
        throw std::runtime_error("File stream error during character read.");
    }

    return nextChar;
}