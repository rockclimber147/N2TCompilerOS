#ifndef COMPILATION_ENGINE_HPP
#define COMPILATION_ENGINE_HPP

#include <string>
#include <vector>
#include <filesystem>
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/SymbolTable.hpp"

namespace fs = std::filesystem;

class CompilationEngine {
public:
    explicit CompilationEngine(const std::string& inputPathStr, bool debugMode = false);
    
    void compile();

private:
    void collectJackFiles(const fs::path& inputPath);
    void debugPrint(const std::string& message);
    
    bool debugMode_;
    std::vector<fs::path> jackFilePaths_;
    ProjectSymbolTable globalTable_;
};

#endif