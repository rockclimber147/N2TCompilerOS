#include "Emulators/VMEmulator/VMParser.hpp"

void VMParser::loadFile(const std::string& filepath) {
    std::vector<std::string> rawLines = loader.loadRawLines(filepath);

    for (const std::string& line : rawLines) {
        std::string cleaned = cleanLine(line);
        
        if (!cleaned.empty()) {
            instructions.push_back(cleaned);
        }
    }
}

void VMParser::clear() {
    instructions.clear();
}

std::string VMParser::cleanLine(const std::string& line) {
    size_t commentPos = line.find("//");
    std::string stripped = (commentPos == std::string::npos) 
                           ? line 
                           : line.substr(0, commentPos);

    const std::string whitespace = " \t\n\r\f\v";
    
    size_t start = stripped.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }

    size_t end = stripped.find_last_not_of(whitespace);
    return stripped.substr(start, end - start + 1);
}