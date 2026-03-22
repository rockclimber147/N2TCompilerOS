#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>

struct FileRange {
    int16_t startAddress;
    std::string fileName;
};

class SymbolTable {
public:
    void addLabel(const std::string& fileName, const std::string& labelName, int16_t address);
    
    // Called once per file at the start of loading
    void registerFileRange(const std::string& fileName, int16_t startAddress);

    int16_t getAddressFromLabel(int16_t currentPC, const std::string& labelName) const;
    void clear();

private:
    // fileName -> { labelName -> address }
    std::unordered_map<std::string, std::unordered_map<std::string, int16_t>> labels;

    // Small vector: {0, "Main"}, {20, "Sys"}, {50, "Class"}
    std::vector<FileRange> fileRanges;

    std::string getFileNameFromPC(int16_t pc) const;
};

#endif