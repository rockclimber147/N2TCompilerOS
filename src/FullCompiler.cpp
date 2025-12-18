#include "FullCompiler.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <filesystem>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

std::string FullCompiler::ensureTrailingSeparator(const std::string& path) const {
    if (path.empty() || path.back() == '/' || path.back() == '\\') {
        return path;
    }
    return path + "/";
}

void FullCompiler::setProjectCWD() {
    try {
        std::filesystem::path currentExeDir = std::filesystem::current_path();
        std::filesystem::path projectRootPath = currentExeDir; 
        
        if (currentExeDir.filename() == "build") {
            projectRootPath = currentExeDir.parent_path(); 
        } else if (currentExeDir.parent_path().filename() == "build") {
            projectRootPath = currentExeDir.parent_path().parent_path();
        } else {
             projectRootPath = currentExeDir.parent_path(); 
        }

        std::filesystem::current_path(projectRootPath); 
        cout << "CWD set to project root: " << std::filesystem::current_path().string() << endl;

    } catch (const std::exception& e) {
        cerr << "Warning: Could not set CWD. File paths may be incorrect. " << e.what() << endl;
    }
}

// --- Constructor: Only stores config ---

FullCompiler::FullCompiler(CompilerConfig config)
    : config_(std::move(config)) // Store config
{
    setProjectCWD();
    cout << "Compiler initialized for file: " << config_.InputFile << endl;
}

void FullCompiler::runAssembler() {
    cout << "--- Starting Hack Assembly ---" << endl;
    
    std::string asmInputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.VMTranslatorOutputDir;
    std::string hackOutputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.HackAssemblerOutputDir;
    std::string baseFileName = removeExtension(config_.InputFile, ".vm");
    baseFileName = removeExtension(baseFileName, ".jack");

    HackAssembler assembler(
        baseFileName, 
        asmInputDir,
        hackOutputDir,
        config_.HackAssemblerDebug,
        config_.HackAssemblerGenerateListing
    );

    assembler.assemble();
    assembler.closeWriters();
    
    cout << "Hack Assembly Complete. Output: " << hackOutputDir << baseFileName << ".hack" << endl;
}


void FullCompiler::runVMTranslator(const std::string& customInputPath) {
    cout << "--- Starting VM Translation ---" << endl;

    std::string asmOutputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.VMTranslatorOutputDir;

    VMTranslator translator(
        customInputPath, 
        asmOutputDir, 
        config_.VMDebug
    );

    translator.translate();
    cout << "VM Translation Complete." << endl;
}

std::string FullCompiler::removeExtension(const std::string& filename, const std::string& ext) {
    if (filename.length() >= ext.length() && 
        filename.substr(filename.length() - ext.length()) == ext) {
        return filename.substr(0, filename.length() - ext.length());
    }
    return filename;
}

void FullCompiler::runCompiler() {
cout << "--- Starting Jack Compilation (.jack -> .vm) ---" << endl;

    std::string jackOutputDir = ensureTrailingSeparator(config_.RootOutputDir) + config_.JackCompilerOutputDir;
    std::string inputPath = ensureTrailingSeparator(config_.InputFolder) + config_.InputFile;

    try {
        // Initialize the engine with the path and debug setting
        CompilationEngine jackEngine(inputPath, jackOutputDir, config_.JackDebug, config_.JackValidateSemantics);
        jackEngine.compile();
    } catch (const std::exception& e) {
        throw std::runtime_error("Jack Compiler Error: " + std::string(e.what()));
    }

    cout << "Jack Compilation Complete." << endl;
}

void FullCompiler::run() {
    cout << "\n--- FullCompiler Execution Start ---" << endl;

    // We start with the user's initial input
    std::string currentInputPath = ensureTrailingSeparator(config_.InputFolder) + config_.InputFile;

    try {
        switch (config_.command) {
            case Command::COMPILE: {
                runCompiler();
                currentInputPath = ensureTrailingSeparator(config_.RootOutputDir) + config_.JackCompilerOutputDir;

                std::string input = ensureTrailingSeparator(config_.InputFolder) + config_.InputFile;
                if (!std::filesystem::is_directory(input)) {
                    std::string base = removeExtension(config_.InputFile, ".jack");
                    currentInputPath += base + ".vm";
                } else {
                    currentInputPath += config_.InputFile;
                }

                runVMTranslator(currentInputPath); 
                
                runAssembler(); 
                break;
            }

            case Command::TRANSLATE:
                runVMTranslator(currentInputPath);
                runAssembler();
                break;

            case Command::ASSEMBLE:
                runAssembler();
                break;

            default:
                throw std::runtime_error("Invalid or unsupported compilation command specified.");
        }
    } catch (const std::exception& e) {
        cerr << "\n!!! Pipeline Failed: " << e.what() << endl;
    }

    cout << "--- FullCompiler Execution End ---" << endl;
}