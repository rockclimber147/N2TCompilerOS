#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <fstream>
#include <stdexcept>

class FileParser {
public:
    FileParser(const std::string& path);
    
    ~FileParser();

    bool hasMoreChars();

    void reset();
    char peekChar();
    char advanceChar();

private:
    std::string filePath;
    std::ifstream fileStream;

    void openFile(const std::string& filePath);
};

#endif