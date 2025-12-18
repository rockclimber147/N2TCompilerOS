#include "JackCompiler/CompilationEngine.hpp"
#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/SemanticAnalyzer.hpp"
#include "JackCompiler/CodeGenerator.hpp"
#include "JackCompiler/VMWriter.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "Tokenizer/Tokenizer.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include <iostream>

CompilationEngine::CompilationEngine(const std::string& inputPathStr, 
                                     const std::string& outputDir, 
                                     bool debugMode)
    : outputDir_(outputDir), debugMode_(debugMode) 
{
    fs::path inputPath(inputPathStr);
    collectJackFiles(inputPath);

    if (jackFilePaths_.empty()) {
        throw std::runtime_error("JackCompiler: No .jack files found in: " + inputPathStr);
    }
    
    // Ensure the output directory exists
    if (!fs::exists(outputDir_)) {
        fs::create_directories(outputDir_);
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
        StreamTokenizer st(spec);
        st.loadFromFile(path.string());
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