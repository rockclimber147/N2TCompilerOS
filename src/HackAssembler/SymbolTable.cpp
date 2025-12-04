#include "HackAssembler/SymbolTable.hpp"
#include <iostream>
#include <sstream>

std::unordered_map<std::string, int> createBuiltInMap() {
    std::unordered_map<std::string, int> builtIns;
    
    // R0-R15 registers
    for (int i = 0; i <= 15; ++i) {
        builtIns["R" + std::to_string(i)] = i;
    }
    
    // Predefined symbols
    builtIns["SCREEN"] = SymbolTable::SCREEN_ADDRESS;
    builtIns["KBD"] = SymbolTable::KBD_ADDRESS;
    builtIns["SP"] = 0;
    builtIns["LCL"] = 1;
    builtIns["ARG"] = 2;
    builtIns["THIS"] = 3;
    builtIns["THAT"] = 4;
    
    return builtIns;
}

SymbolTable::SymbolTable() 
    : nextVariableAddress_(START_VARIABLE_ADDRESS) 
{
    symbolTable_ = createBuiltInMap();
}

bool SymbolTable::hasSymbol(const std::string& symbol) const {
    return symbolTable_.count(symbol) > 0;
}

int SymbolTable::getAddress(const std::string& symbol) {
    if (!hasSymbol(symbol)) {
        throw std::out_of_range("Symbol not found in table: " + symbol);
    }
    return symbolTable_.at(symbol); 
}

void SymbolTable::addJumpLabel(const std::string& label, int address) {
    symbolTable_.emplace(label, address);
}

void SymbolTable::addVariable(const std::string& label) {
    symbolTable_.emplace(label, nextVariableAddress_);
    nextVariableAddress_++; 
}