#include <iostream>
#include <string>
#include <stdexcept>
#include "FullCompiler.hpp" 

using namespace std;

// FullCompiler sets CWD is set to the project root.
const string FILENAME = "Test";
const string INPUT_DIR = "__input__/";
const string OUTPUT_DIR = "__output__/"; 

const string usage = "usage: executable -c/-t/-a filename/folder\n";


int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            cout << usage;
            exit(1);
        }

        CompilerConfig config;
        std::string commandArg = argv[1];

        if (commandArg == "-c") {
            config.command = Command::COMPILE;
        } else if (commandArg == "-t") {
            config.command = Command::TRANSLATE;
        } else if (commandArg == "-a") {
            config.command = Command::ASSEMBLE;
        } else {
            cout << usage;
            exit(1);
        }
        config.InputFolder = INPUT_DIR;
        config.RootOutputDir = OUTPUT_DIR; 

        std::string fileArg = argv[2];
        config.InputFile = fileArg;

        for (int i = 3; i < argc; ++i) {
            string arg = argv[i];

            if (arg == "--debug" || arg == "-d") {
                config.JackDebug = true;
                config.VMDebug = true;
                config.HackAssemblerDebug = true;
            } else if (arg == "--validate" || arg == "-v") {
                config.JackValidateSemantics = true;
            } else if (arg == "--listing" || arg == "-l") {
                config.HackAssemblerGenerateListing = true;
            } else if (arg == "--xml") {
                config.JackCompilerGenerateXML = true;
            } else if (arg[0] == '-') {
                cerr << "Unknown flag: " << arg << endl;
            } else {
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