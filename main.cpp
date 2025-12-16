#include <iostream>
#include <string>
#include <stdexcept>
#include "FullCompiler.hpp" 

using namespace std;

// FullCompiler sets CWD is set to the project root.
const string FILENAME = "Sys.vm";
const string INPUT_DIR = "__input__/";
const string OUTPUT_DIR = "__output__/"; 


int main(int argc, char* argv[]) {
    try {
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

        cout << "\n--- Starting Full Compilation: " << FILENAME << " ---" << endl;

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