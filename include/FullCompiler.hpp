#ifndef FULLCOMPILER_HPP
#define FULLCOMPILER_HPP

#include <string>
#include "VMTranslator/VMTranslator.hpp"
#include "HackAssembler/HackAssembler.hpp"
#include "JackCompiler/CompilationEngine.hpp" // Added Include

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
    
    std::string VMTranslatorOutputDir = "asm/"; 
    std::string HackAssemblerOutputDir = "hack/"; 
    std::string JackCompilerOutputDir = "jack/";

    bool JackDebug = false;
    bool JackCompilerGenerateXML = false;
    bool JackValidateSemantics = false;
    bool VMDebug = false;
    bool HackAssemblerDebug = false;
    bool HackAssemblerGenerateListing = false;
};

class FullCompiler {
private:
    CompilerConfig config_;
    
    static void setProjectCWD();
    std::string ensureTrailingSeparator(const std::string& path) const; 

    void runVMTranslator(const std::string& customInputPath);
    void runAssembler();
    void runCompiler();
    std::string removeExtension(const std::string& filename, const std::string& ext);
public:
    FullCompiler(CompilerConfig config); 
    void run();
};

#endif