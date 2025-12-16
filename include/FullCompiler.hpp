#ifndef FULLCOMPILER_HPP
#define FULLCOMPILER_HPP

#include <string>
// We still need the forward declarations/includes for the private methods to work
#include "VMTranslator/VMTranslator.hpp"
#include "HackAssembler/HackAssembler.hpp"

enum class Command {
    COMPILE,
    TRANSLATE,
    ASSEMBLE
};

struct CompilerConfig {
    Command command;

    std::string InputFile;
    std::string InputFolder;
    std::string RootOutputDir;
    
    // The relative subfolders for output
    std::string VMTranslatorOutputDir = "asm/"; 
    std::string HackAssemblerOutputDir = "hack/"; 

    bool VMDebug = false;
    bool HackAssemblerDebug = false;
    bool HackAssemblerGenerateListing = false;
};

class FullCompiler {
private:
    CompilerConfig config_;
    
    static void setProjectCWD();
    std::string ensureTrailingSeparator(const std::string& path) const; 

    void runAssembler();
    void runVMTranslator();
    void runCompiler();

public:
    FullCompiler(CompilerConfig config); 
    void run();
};

#endif // FULLCOMPILER_HPP