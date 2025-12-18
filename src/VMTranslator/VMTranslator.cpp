#include "VMTranslator/VMTranslator.hpp"
#include "VMTranslator/VMCommandParser.hpp"
#include "VMTranslator/VMCodeWriter.hpp"
#include "parser.hpp"
#include <iostream>
#include <stdexcept>
#include <memory>
#include <filesystem>

using std::string;

VMTranslator::VMTranslator(const string& inputPathStr, const string& outputDir, const bool debugMode)
    : debugMode_(debugMode)
{
    fs::path inputPath(inputPathStr);
    try {
        collectVmFiles(inputPath);
    } catch (const std::runtime_error& e) {
        throw e;
    }
    
    if (vmFilePaths_.empty()) {
        throw std::runtime_error("VM Translator: No .vm files found in the specified path: " + inputPathStr);
    }

    fs::path outPath(outputDir);
    if (!fs::exists(outPath)) {
        debugPrint("Creating output directory: " + outputDir);
        fs::create_directories(outPath); 
    }
    
    string outputBaseName;
    if (fs::is_directory(inputPath)) {
        outputBaseName = inputPath.filename().string();
    } else {
        outputBaseName = inputPath.stem().string(); 
    }
    
    string asmFilePath = fs::path(outputDir) / (outputBaseName + ".asm");
    
    codeWriter_ = std::make_unique<VMCodeWriter>(asmFilePath);
    currentFile_ = outputBaseName; 

    debugPrint("VM Translator initialized. Input: " + inputPathStr + ", Output: " + asmFilePath);
}

void VMTranslator::collectVmFiles(const fs::path& inputPath) {
    
    if (fs::is_regular_file(inputPath) && inputPath.extension() == ".vm") {
        vmFilePaths_.push_back(inputPath);
    } else if (fs::is_directory(inputPath)) {
        std::vector<fs::path> collectedFiles;
        try {
            for (const auto& entry : fs::directory_iterator(inputPath)) {
                if (entry.path().extension() == ".vm") {
                    collectedFiles.push_back(entry.path());
                }
            }
        } catch (const fs::filesystem_error& e) {
            throw std::runtime_error("Filesystem error while traversing directory: " + std::string(e.what()));
        }
        if (collectedFiles.empty()) {
            throw std::runtime_error("Directory contains no .vm files: " + inputPath.string());
        }
        auto sysFileIt = std::find_if(collectedFiles.begin(), collectedFiles.end(),
            [](const fs::path& p) {
                return p.filename() == "Sys.vm";
            });

        if (sysFileIt != collectedFiles.end()) {
            vmFilePaths_.push_back(*sysFileIt);

            for (const auto& p : collectedFiles) {
                if (p.filename() != "Sys.vm") {
                    vmFilePaths_.push_back(p);
                }
            }
            std::sort(vmFilePaths_.begin() + 1, vmFilePaths_.end());

        } else {
            std::sort(collectedFiles.begin(), collectedFiles.end());
            vmFilePaths_ = std::move(collectedFiles);
        }

    } else {
        throw std::runtime_error("Input path is neither a .vm file nor a directory: " + inputPath.string());
    }
}


void VMTranslator::translate() {
    if (!vmFilePaths_.empty()) {
        codeWriter_->writeInit(); // Note: You need to implement writeInit() in VMCodeWriter
        debugPrint("Wrote Bootstrap Code (SP=256, call Sys.init).");
    }

    if (!vmFilePaths_.empty() && vmFilePaths_[0].filename() == "Sys.vm") {
        codeWriter_->writeAsComment("--- BOOTSTRAP: Call Sys.init ---");
        codeWriter_->writeCall("Sys.init", 0);
        debugPrint("Wrote Bootstrap call to Sys.init.");
    }

    for (const auto& vmFilePath : vmFilePaths_) {
        translateSingleFile(vmFilePath);
    }
    
    closeWriter();
}

void VMTranslator::translateSingleFile(const fs::path& vmFilePath) {
    string fileName = vmFilePath.stem().string();
    codeWriter_->setFileName(fileName);
    currentFile_ = fileName;
    
    codeWriter_->writeFileName(fileName);
    debugPrint("Starting translation of: " + fileName + ".vm");

    std::unique_ptr<Parser> fileParser;
    try {
        fileParser = std::make_unique<Parser>(vmFilePath.string()); 
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("VM Translator failed to open/load input: " + vmFilePath.string() + ": " + string(e.what()));
    }

    int codeLineNo = 0;
    while (fileParser->hasMoreLines()) {
        string rawLine = fileParser->advance();
        codeLineNo++;
        VMCommandParser currentCommand(rawLine);
        
        VMSpecifications::CommandType commandType = currentCommand.type();
        codeWriter_->writeAsComment(rawLine);
        
        if (commandType == VMSpecifications::CommandType::C_INVALID) {
             continue; 
        }
        
        if (commandType == VMSpecifications::CommandType::C_ARITHMETIC) {
            codeWriter_->writeArithmetic(currentCommand.arg1());
            
        } else if (commandType == VMSpecifications::CommandType::C_PUSH || 
                   commandType == VMSpecifications::CommandType::C_POP) 
        {
            const string& segment = currentCommand.arg1();
            int index = currentCommand.arg2();
            
            if (commandType == VMSpecifications::CommandType::C_PUSH) {
                codeWriter_->writePush(segment, index);
            } else {
                codeWriter_->writePop(segment, index);
            }
        } else if (commandType == VMSpecifications::CommandType::C_LABEL) {
            codeWriter_->writeLabel(currentCommand.arg1());
            
        } else if (commandType == VMSpecifications::CommandType::C_GOTO) {
            codeWriter_->writeGoTo(currentCommand.arg1());
            
        } else if (commandType == VMSpecifications::CommandType::C_IF_GOTO) {
            codeWriter_->writeIf(currentCommand.arg1());
            
        } else if (commandType == VMSpecifications::CommandType::C_FUNCTION) {
            codeWriter_->writeFunction(currentCommand.arg1(), currentCommand.arg2());
            
        } else if (commandType == VMSpecifications::CommandType::C_CALL) {
            codeWriter_->writeCall(currentCommand.arg1(), currentCommand.arg2());
            
        } else if (commandType == VMSpecifications::CommandType::C_RETURN) {
            codeWriter_->writeReturn();
            
        }
        
        debugPrint("Line " + std::to_string(codeLineNo) + " translated: " + currentCommand.getCleanCommand());
    }
}

void VMTranslator::closeWriter() {
    if (codeWriter_) {
        codeWriter_->close();
    }
}

void VMTranslator::debugPrint(const std::string &message) {
    if (debugMode_) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}