#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

class SymbolTable {
public:
    static const int START_VARIABLE_ADDRESS = 16;
    static const int SCREEN_ADDRESS = 16384;
    static const int KBD_ADDRESS = 24576;
    
    SymbolTable();

    bool hasSymbol(const std::string& symbol) const;
    
    int getAddress(const std::string& symbol);
    
    void addJumpLabel(const std::string& label, int address);
    
    void addVariable(const std::string& label);

private:
    std::unordered_map<std::string, int> symbolTable_;
    int nextVariableAddress_;
    
    void initializeBuiltInSymbols(); 
};