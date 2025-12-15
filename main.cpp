#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "parser.hpp" // Included for context, but not used by VMTranslator directly now
#include "VMTranslator/VMTranslator.hpp" // <-- NEW INCLUDE

using namespace std;

// --- Define file and directory constants ---
const string FILENAME = "BasicTest";
const string INPUT_DIR = "../__input__/";
const string OUTPUT_DIR = "../__output__"; 


int main(int argc, char* argv[]) {
    // We will ignore command-line arguments (argc, argv) for this fixed test.
    try {
        cout << __FILE__ << endl;
        cout << "--- Starting VM Translation: " << FILENAME << ".vm ---" << endl;
        
        // 1. Instantiate the VMTranslator
        // VMTranslator(fileName, inputDir, outputDir, debugMode)
        VMTranslator translator(
            FILENAME, 
            INPUT_DIR, 
            OUTPUT_DIR, 
            true // Enable Debug Mode for testing
        );

        // 2. Execute the translation process
        translator.translate();

        // 3. Close and confirm
        cout << "\n--- Translation Complete ---" << endl;
        cout << "Input: " << INPUT_DIR << FILENAME << ".vm" << endl;
        cout << "Output: " << OUTPUT_DIR << "/asm/" << FILENAME << ".asm" << endl;
        
    }
    catch (const std::runtime_error& e) {
        cerr << "\n!!! Fatal Runtime Error during Translation !!!" << endl;
        cerr << "Error: " << e.what() << endl;
        return 1; // Return non-zero to indicate failure
    }
    // Return 0 if successful
    return 0;
}