#include "Emulators/VMEmulator/VMParser.hpp"

void VMParser::loadFile(const std::string& filepath, SymbolTable& table) {
    std::string fileName = std::filesystem::path(filepath).stem().string();
    table.registerFileRange(fileName, static_cast<int16_t>(instructions.size()));

    std::vector<std::string> rawLines = loader.loadRawLines(filepath);

    for (const std::string& line : rawLines) {
        std::string cleaned = cleanLine(line);
        
        if (cleaned.empty()) {
            continue;
        }

        if (cleaned.rfind("label ", 0) == 0) {

            std::string labelName = cleaned.substr(6);
            size_t last = labelName.find_last_not_of(" \t\n\r");
            if (last != std::string::npos) {
                labelName = labelName.substr(0, last + 1);
            }

            table.addLabel(fileName, labelName, static_cast<int16_t>(instructions.size()));
        } 
        else if (cleaned.rfind("function ", 0) == 0) {
            std::stringstream ss(cleaned.substr(9));
            std::string funcName;
            int16_t locals = 0;

            if (ss >> funcName >> locals) {
                table.addFunction(funcName, static_cast<int16_t>(instructions.size()), locals);
            }
        }
        else {
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