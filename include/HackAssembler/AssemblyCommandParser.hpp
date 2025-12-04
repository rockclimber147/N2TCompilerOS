#pragma once

#include <string>

class AssemblyCommandParser {
public:
    enum CommandType {
        A_INSTRUCTION,
        C_INSTRUCTION,
        L_INSTRUCTION,
        NO_COMMAND
    };

    explicit AssemblyCommandParser(const std::string& line);

    CommandType type() const;
    
    // Parsing methods
    std::string symbol() const;
    std::string dest() const;
    std::string comp() const;
    std::string jump() const;

private:
    std::string cleanLine; // The stripped, no-comment part of the line
    void cleanAndSetLine(const std::string& line);
};