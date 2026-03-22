#ifndef VM_PARSER_HPP
#define VM_PARSER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include "Emulators/FileLoader.hpp"
#include "Emulators/VMEmulator/SymbolTable.hpp"

class VMParser {
public:
    VMParser() = default;
    void loadFile(const std::string& filepath, SymbolTable& table);
    void clear();
    const std::vector<std::string>& getInstructions() const { return instructions; }
    std::string cleanLine(const std::string& line);
    
private:
    FileLoader loader;
    std::vector<std::string> instructions;
};

#endif