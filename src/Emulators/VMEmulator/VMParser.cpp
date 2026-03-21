#include "VMParser.hpp"
#include <algorithm>

std::vector<std::string> VMParser::stripCommentsAndWhiteSpace(const std::vector<std::string>& rawLines) {
    std::vector<std::string> cleanInstructions;

    for (const auto& line : rawLines) {
        std::string cleaned = cleanLine(line);
        if (!cleaned.empty()) {
            cleanInstructions.push_back(cleaned);
        }
    }

    return cleanInstructions;
}

std::string VMParser::cleanLine(const std::string& line) {
    // 1. Strip comments
    size_t commentPos = line.find("//");
    std::string stripped = (commentPos == std::string::npos) ? line : line.substr(0, commentPos);

    // 2. Trim leading/trailing whitespace
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = stripped.find_first_not_of(whitespace);
    if (start == std::string::npos) return ""; // Line was only whitespace/comments

    size_t end = stripped.find_last_not_of(whitespace);
    return stripped.substr(start, end - start + 1);
}