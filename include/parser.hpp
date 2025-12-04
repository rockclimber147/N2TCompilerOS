#pragma once

#include <string>
#include <vector>

class Parser {
public:
    const std::string filePath;

    Parser(const std::string& path);

    bool hasMoreLines() const;
    std::string advance();

protected:
    size_t currentIndex;
    std::vector<std::string> lines;

    void loadFile(const std::string& filePath);
};