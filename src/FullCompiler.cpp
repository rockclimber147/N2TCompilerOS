// FullCompiler.cpp

#include "FullCompiler.hpp"
#include <iostream>
#include <stdexcept>
#include <utility> // For std::move

using std::string;
using std::cout;
using std::cerr;
using std::endl;

// Helper function definition
std::string FullCompiler::ensureTrailingSeparator(const std::string& path) const {
    if (path.empty() || path.back() == '/' || path.back() == '\\') {
        return path;
    }
    return path + "/";
}

// --- Constructor: Only stores config ---

FullCompiler::FullCompiler(CompilerConfig config)
    : config_(std::move(config)) // Store config
{
    cout << "Compiler initialized for file: " << config_.InputFile << endl;
}

void FullCompiler::runVMTranslator() {
    cout << "--- Starting VM Translation ---" << endl;

    std::string asmOutputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.VMTranslatorOutputDir;

    VMTranslator translator(
        config_.InputFile, 
        ensureTrailingSeparator(config_.InputFolder), 
        asmOutputDir, 
        config_.VMDebug
    );

    translator.translate();
    
    cout << "VM Translation Complete. Output: " << asmOutputDir << config_.InputFile << ".asm" << endl;
}


void FullCompiler::runAssembler() {
    cout << "--- Starting Hack Assembly ---" << endl;
    
    std::string asmInputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.VMTranslatorOutputDir;
    std::string hackOutputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.HackAssemblerOutputDir;

    HackAssembler assembler(
        config_.InputFile, 
        asmInputDir,
        hackOutputDir,
        config_.HackAssemblerDebug,
        config_.HackAssemblerGenerateListing
    );

    assembler.assemble();
    assembler.closeWriters();
    
    cout << "Hack Assembly Complete. Output: " << hackOutputDir << config_.InputFile << ".hack" << endl;
}


void FullCompiler::runCompiler() {
    runVMTranslator();
    runAssembler();
}

void FullCompiler::run() {
    cout << "\n--- FullCompiler Execution Start ---" << endl;

    switch (config_.command) {
        case Command::TRANSLATE:
            runVMTranslator();
            runAssembler();
            break;
            
        case Command::ASSEMBLE:
            runAssembler();
            break;
            
        case Command::COMPILE:
            runCompiler();
            runVMTranslator();
            runAssembler();
            break;
            
        default:
            throw std::runtime_error("Invalid or unsupported compilation command specified.");
    }
    
    cout << "--- FullCompiler Execution End ---" << endl;
}