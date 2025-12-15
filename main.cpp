#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "VMTranslator/VMTranslator.hpp"
#include "HackAssembler/HackAssembler.hpp"

using namespace std;

// --- Define file and directory constants ---
const string FILENAME = "BasicTest";
const string INPUT_DIR = "../__input__/";
const string OUTPUT_DIR = "../__output__/"; 


int main(int argc, char* argv[]) {
    try {
        cout << "--- Starting VM Translation: " << FILENAME << ".vm ---" << endl;
        VMTranslator translator(
            FILENAME, 
            INPUT_DIR, 
            OUTPUT_DIR + "asm/", 
            false
        );

        translator.translate();

        // 3. Close and confirm
        cout << "\n--- Translation Complete ---" << endl;
        cout << "Input: " << INPUT_DIR << FILENAME << ".vm" << endl;
        cout << "Output: " << OUTPUT_DIR << "/asm/" << FILENAME << ".asm" << endl;

                cout << "--- Starting Assembly: " << FILENAME << ".vm ---" << endl;
        HackAssembler assembler(
            FILENAME, 
            OUTPUT_DIR + "asm/", 
            OUTPUT_DIR + "hack/", 
            true
        );

        assembler.assemble();

        // 3. Close and confirm
        cout << "\n--- Assembly ---" << endl;
        cout << "Input: " << OUTPUT_DIR << "/asm/" << FILENAME << ".asm" << endl;
        cout << "Output: " << OUTPUT_DIR << "/asm/" << FILENAME << ".hack" << endl;
        
    }
    catch (const std::runtime_error& e) {
        cerr << "\n!!! Fatal Runtime Error during Translation !!!" << endl;
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    // Return 0 if successful
    return 0;
}