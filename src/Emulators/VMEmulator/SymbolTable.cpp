#include "Emulators/VMEmulator/SymbolTable.hpp"
#include <algorithm>

void SymbolTable::addLabel(const std::string& fileName, const std::string& labelName, int16_t address) {
    labels[fileName][labelName] = address;
}

void SymbolTable::addFunction(const std::string& functionName, int16_t address, int16_t locals) {
    functions[functionName] = { address, locals };
}

void SymbolTable::registerFileRange(const std::string& fileName, int16_t startAddress) {
    fileRanges.push_back({startAddress, fileName});
}

std::string SymbolTable::getFileNameFromPC(int16_t pc) const {
    if (fileRanges.empty()) return "";

    auto it = std::upper_bound(fileRanges.begin(), fileRanges.end(), pc, 
        [](int16_t val, const FileRange& range) {
            return val < range.startAddress;
        });

    if (it != fileRanges.begin()) {
        return std::prev(it)->fileName;
    }
    
    return fileRanges.front().fileName;
}

int16_t SymbolTable::getAddressFromLabel(int16_t currentPC, const std::string& labelName) const {
    std::string currentFile = getFileNameFromPC(currentPC);
    
    auto fileIt = labels.find(currentFile);
    if (fileIt != labels.end()) {
        auto labelIt = fileIt->second.find(labelName);
        if (labelIt != fileIt->second.end()) {
            return labelIt->second;
        }
    }
    throw std::runtime_error("Label not found: " + labelName);
}

FunctionEntry SymbolTable::getFunctionAddress(std::string& functionName) const {
    auto it = functions.find(functionName);
    
    if (it != functions.end()) {
        return it->second;
    }
    
    throw std::runtime_error("Function not found: " + functionName);
}

void SymbolTable::clear() {
    labels.clear();
    fileRanges.clear();
}