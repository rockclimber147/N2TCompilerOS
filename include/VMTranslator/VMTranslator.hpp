#include <string>
#include <stdexcept>
#include <memory>
#include <filesystem>
#include <vector>
#include "VMTranslator/VMCodeWriter.hpp"

namespace fs = std::filesystem;

class VMTranslator {
private:
    std::vector<fs::path> vmFilePaths_;
    std::unique_ptr<VMCodeWriter> codeWriter_;
    std::string currentFile_;
    bool debugMode_;

    void collectVmFiles(const fs::path& inputPath);
    void translateSingleFile(const fs::path& vmFilePath);
    void closeWriter();
    void debugPrint(const std::string& message); 

public:
    // Constructor now takes a path that can be a file or a folder
    VMTranslator(const std::string& inputPath, const std::string& outputDir, const bool debugMode);
    
    void translate();
};