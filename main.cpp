#include <iostream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include "FullCompiler.hpp" 

using namespace std;

const string FILENAME = "BasicTest";
const string INPUT_DIR = "__input__/";
const string OUTPUT_DIR = "__output__/"; 


void setProjectCWD() {
    try {
        std::filesystem::path currentExeDir = std::filesystem::current_path();
        std::filesystem::path projectRootPath = currentExeDir.parent_path();
        
        if (currentExeDir.string().find("build") != string::npos) {
            projectRootPath = currentExeDir.parent_path();
        }
        
        std::filesystem::current_path(projectRootPath); 
        cout << "CWD set to project root: " << std::filesystem::current_path().string() << endl;

    } catch (const std::exception& e) {
        cerr << "Warning: Could not set CWD. File paths may be incorrect. " << e.what() << endl;
    }
}


int main(int argc, char* argv[]) {
    try {
        setProjectCWD();
        CompilerConfig config;
        
        config.command = Command::COMPILE;
        config.InputFile = FILENAME;
        config.InputFolder = INPUT_DIR;
        config.RootOutputDir = OUTPUT_DIR; 

        config.VMTranslatorOutputDir = "asm/"; 
        config.HackAssemblerOutputDir = "hack/"; 

        config.VMDebug = false;
        config.HackAssemblerDebug = false;
        config.HackAssemblerGenerateListing = true;

        cout << "\n--- Starting Full Compilation: " << FILENAME << ".vm ---" << endl;

        FullCompiler compiler(config);
        compiler.run();
        
    }
    catch (const std::runtime_error& e) {
        cerr << "\n!!! FATAL COMPILER ERROR !!!" << endl;
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}