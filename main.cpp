#include <iostream>
#include <string>
#include <stdexcept>
#include "FullCompiler.hpp" 

using namespace std;

// FullCompiler sets CWD is set to the project root.
const string FILENAME = "Test";
const string INPUT_DIR = "__input__/";
const string OUTPUT_DIR = "__output__/"; 


int main(int argc, char* argv[]) {
    try {
        CompilerConfig config;
        
        // Default settings
        config.command = Command::COMPILE;
        config.InputFolder = INPUT_DIR;
        config.RootOutputDir = OUTPUT_DIR; 
        config.InputFile = FILENAME;

        // Argument Parsing
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];

            if (arg == "--debug" || arg == "-d") {
                config.JackDebug = true;
                config.VMDebug = true;
                config.HackAssemblerDebug = true;
            } else if (arg == "--no-validate") {
                config.JackValidateSemantics = false;
            } else if (arg == "--validate" || arg == "-v") {
                config.JackValidateSemantics = true;
            } else if (arg == "--listing" || arg == "-l") {
                config.HackAssemblerGenerateListing = true;
            } else if (arg == "--xml") {
                config.JackCompilerGenerateXML = true;
            } else if (arg[0] == '-') {
                cerr << "Unknown flag: " << arg << endl;
            } else {
                // If it doesn't start with '-', assume it's the InputFile
                config.InputFile = arg;
            }
        }

        cout << "\n--- Starting Full Compilation: " << config.InputFile << " ---" << endl;
        if (config.JackDebug) cout << "[Mode: Debug Enabled]" << endl;

        FullCompiler compiler(config);
        compiler.run();
        
    }
    catch (const std::exception& e) { // Catch all standard exceptions
        cerr << "\n!!! FATAL COMPILER ERROR !!!" << endl;
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}