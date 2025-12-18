#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "JackCompiler/Commontypes.hpp"

struct ClassEntry;

struct VariableSymbol {
    std::string name;
    std::string type;
    VarKind kind;
    int index;
};

struct SubroutineEntry {
    std::string name;
    std::string returnType;
    SubroutineType routineType;
    
    std::unordered_map<std::string, VariableSymbol> symbolTable;
    
    int argCounter = 0;
    int varCounter = 0;

    void define(const std::string& name, const std::string& type, VarKind kind) {
        int index = (kind == VarKind::ARG) ? argCounter++ : varCounter++;
        symbolTable[name] = {name, type, kind, index};
    }

    const VariableSymbol* lookup(const std::string& name, const ClassEntry* parentClass) const;
};

struct ClassEntry {
    std::string className;
    std::unordered_map<std::string, VariableSymbol> classSymbols;
    std::unordered_map<std::string, SubroutineEntry> subroutines;

    int fieldCounter = 0;
    int staticCounter = 0;

    void define(const std::string& name, const std::string& type, VarKind kind) {
        int index = (kind == VarKind::FIELD) ? fieldCounter++ : staticCounter++;
        classSymbols[name] = {name, type, kind, index};
    }

    const VariableSymbol* lookup(const std::string& name) const {
        auto it = classSymbols.find(name);
        if (it != classSymbols.end()) return &it->second;
        return nullptr;
    }
};

inline const VariableSymbol* SubroutineEntry::lookup(const std::string& name, const ClassEntry* parentClass) const {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) return &it->second;

    if (parentClass) return parentClass->lookup(name);

    return nullptr;
}

class ProjectSymbolTable {
public:
    void addClass(const std::string& name) {
        classes[name] = ClassEntry{name};
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