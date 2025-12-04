#include <stdexcept>
#include <fstream>
#include "parser.hpp"

Parser::Parser(const std::string& path)
    : filePath(path), currentIndex(0) {
    loadFile(path);
}

bool Parser::hasMoreLines() const {
    return currentIndex < lines.size();
}

void Parser::resetIndex() {
    currentIndex = 0;
}

std::string Parser::advance() {
    if (!hasMoreLines()) {
        throw std::out_of_range("no more lines");
    }
    return lines[currentIndex++];
}

std::string Parser::getCurrentLine() {
    if (!hasMoreLines()) {
        throw std::out_of_range("no more lines");
    }
    return lines[currentIndex];
}

void Parser::loadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
}