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
    
    // VMTranslator and HackAssembler members are REMOVED!

    // Helper to ensure paths end with a separator (like '/') for safety
    std::string ensureTrailingSeparator(const std::string& path) const; 

    // Execution methods will now create and run their respective tools
    void runAssembler();
    void runVMTranslator();
    void runCompiler();

public:
    FullCompiler(CompilerConfig config); 
    void run();
};

#endif // FULLCOMPILER_HPP