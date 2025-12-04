#include "AssemblyFileParser.hpp"
#include <algorithm>
#include <sstream>


std::string strip(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

CommandParser::CommandParser(const std::string& line) {
    cleanAndSetLine(line);
}

void CommandParser::cleanAndSetLine(const std::string& line) {
    size_t commentPos = line.find("//");
    std::string noCommentPart = (commentPos != std::string::npos) 
                                ? line.substr(0, commentPos) 
                                : line;
    cleanLine = strip(noCommentPart);
}

CommandParser::CommandType CommandParser::type() const {
    if (cleanLine.empty()) {
        return NO_COMMAND;
    }
    if (cleanLine[0] == '@') {
        return A_INSTRUCTION;
    }
    if (cleanLine[0] == '(' && cleanLine.back() == ')') {
        return L_INSTRUCTION;
    }
    return C_INSTRUCTION;
}

std::string CommandParser::symbol() const {
    if (type() == A_INSTRUCTION) {
        return cleanLine.substr(1);
    }
    if (type() == L_INSTRUCTION) {
        return cleanLine.substr(1, cleanLine.length() - 2);
    }
    return "";
}

std::string CommandParser::dest() const {
    if (type() != C_INSTRUCTION) return "";
    size_t eqPos = cleanLine.find('=');
    if (eqPos != std::string::npos) {
        return cleanLine.substr(0, eqPos);
    }
    return "";
}