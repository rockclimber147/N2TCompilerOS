#include <iostream>
#include <vector>
#include <string>

using namespace std;


int main(int argc, char* argv[]) {
    std::cout << "Argument count: " << argc << "\n";
    for (int i = 0; i < argc; ++i) {
        std::cout << "arg[" << i << "]: " << argv[i] << "\n";
    }
    return 0;
}