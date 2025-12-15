#ifndef ASSEMBLER_CONFIG_HPP
#define ASSEMBLER_CONFIG_HPP

#include <string>

struct AssemblerConfig {
    std::string fileName;
    std::string inputDir;
    std::string outputDir;
    bool debugMode = false;
    bool generateListing = true;
};

#endif