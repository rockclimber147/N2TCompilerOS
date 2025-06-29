#include <iostream>
#include <vector>
#include <string>
#include "parser.hpp"

using namespace std;


int main(int argc, char* argv[]) {
    std::string filePath = "test.txt";
    Parser p = Parser(filePath);
    cout << argc << "\n";
    while (p.hasMoreLines()) {
        std::cout << p.advance() << "\n";
    }
}