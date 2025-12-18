#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

enum class SymbolKind { STATIC, FIELD, ARG, VAR };

struct VariableSymbol {
    std::string name;
    std::string type;
    SymbolKind kind;
    int index;
};

struct SubroutineEntry {
    std::string name;
    std::string returnType;
    std::string routineType;
    
    std::unordered_map<std::string, VariableSymbol> symbolTable;
    
    int argCounter = 0;
    int varCounter = 0;

    void define(const std::string& name, const std::string& type, SymbolKind kind) {
        int index = (kind == SymbolKind::ARG) ? argCounter++ : varCounter++;
        symbolTable[name] = {name, type, kind, index};
    }
};

struct ClassEntry {
    std::string className;
    
    std::unordered_map<std::string, VariableSymbol> classSymbols;
    
    std::unordered_map<std::string, SubroutineEntry> subroutines;

    int fieldCounter = 0;
    int staticCounter = 0;

    void define(const std::string& name, const std::string& type, SymbolKind kind) {
        int index = (kind == SymbolKind::FIELD) ? fieldCounter++ : staticCounter++;
        classSymbols[name] = {name, type, kind, index};
    }
};

class ProjectSymbolTable {
public:
    void addClass(const std::string& name) {
        classes[name] = {name};
    }

    ClassEntry* getClass(const std::string& name) {
        if (classes.find(name) == classes.end()) return nullptr;
        return &classes[name];
    }

    bool classExists(const std::string& name) const {
        return classes.find(name) != classes.end();
    }

private:
    std::unordered_map<std::string, ClassEntry> classes;
};

#endif