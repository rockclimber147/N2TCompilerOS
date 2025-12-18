#include "JackCompiler/CompilationEngine.hpp"
#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/SemanticAnalyzer.hpp"
#include "JackCompiler/CodeGenerator.hpp"
#include "JackCompiler/VMWriter.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include <iostream>

CompilationEngine::CompilationEngine(const std::string& inputPathStr, bool debugMode)
    : debugMode_(debugMode) 
{
    fs::path inputPath(inputPathStr);
    collectJackFiles(inputPath);

    if (jackFilePaths_.empty()) {
        throw std::runtime_error("No .jack files found in: " + inputPathStr);
    }
}

void CompilationEngine::collectJackFiles(const fs::path& inputPath) {
    if (fs::is_regular_file(inputPath) && inputPath.extension() == ".jack") {
        jackFilePaths_.push_back(inputPath);
    } else if (fs::is_directory(inputPath)) {
        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".jack") {
                jackFilePaths_.push_back(entry.path());
            }
        }
    } else {
        throw std::runtime_error("Invalid input path: " + inputPath.string());
    }
}

void CompilationEngine::compile() {
    std::vector<ClassIR> projectASTs;
    JackSpec spec;

    // --- Pass 1: Parsing ---
    debugPrint("Starting Parsing Pass...");
    for (const auto& path : jackFilePaths_) {
        debugPrint("Parsing: " + path.filename().string());
        StreamTokenizer st(path.string(), spec);
        TokenValidator validator(st);
        JackParser parser(validator);
        projectASTs.push_back(parser.parseClass());
    }

    // --- Pass 2: Semantic Analysis ---
    debugPrint("Starting Semantic Analysis Pass...");
    SemanticAnalyzer analyzer(globalTable_);
    analyzer.analyze(projectASTs);

    // --- Pass 3: Code Generation ---
    debugPrint("Starting Code Generation Pass...");
    for (const auto& classAST : projectASTs) {
        // Output file is always [ClassName].vm in the same directory as input
        // Note: You can modify this to support a specific output directory
        std::string outPath = (fs::path(classAST.name + ".vm")).string();
        
        debugPrint("Generating: " + outPath);
        VMWriter writer(outPath);
        CodeGenerator generator(globalTable_, writer);
        
        // Generate code for this specific class
        generator.writeClass(classAST);
    }

    debugPrint("Compilation complete.");
}

void CompilationEngine::debugPrint(const std::string& message) {
    if (debugMode_) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}