#include <iostream>
#include <vector>
#include <string>
#include "parser.hpp"

using namespace std;


int main(int argc, char* argv[]) {
    try {
        std::string filePath = argv[1];
        if (argc >= 2) {
            filePath = argv[1];
        }
        Parser p = Parser(filePath);
        while (p.hasMoreLines()) {
            std::cout << p.advance() << "\n";
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
    }
}