#ifndef HIGH_LEVEL_IR_HPP
#define HIGH_LEVEL_IR_HPP

#include <string>
#include <vector>
#include <memory>
#include <utility>

// Forward declaration for the body of subroutines
struct StatementIR;

// --- 1. Variable Declarations ---

enum class VarKind {
    STATIC,
    FIELD, // Maps to 'this' pointer in the VM
    ARG,
    VAR    // Local
};

struct VariableIR {
    VarKind kind;
    std::string type;
    std::string name;

    VariableIR(VarKind k, std::string t, std::string n)
        : kind(k), type(std::move(t)), name(std::move(n)) {}
};

// --- 2. Subroutine Declarations ---

enum class SubroutineType {
    CONSTRUCTOR,
    FUNCTION,
    METHOD
};

struct SubroutineIR {
    SubroutineType type;
    std::string returnType; 
    std::string name;
    std::string fullyQualifiedName; 

    std::vector<VariableIR> parameters; 
    std::vector<VariableIR> locals;     
    std::vector<std::unique_ptr<StatementIR>> bodyStatements; 

    SubroutineIR(SubroutineType t, std::string retType, std::string n);
    ~SubroutineIR();
    SubroutineIR(SubroutineIR&&) noexcept;
    SubroutineIR& operator=(SubroutineIR&&) noexcept;
    SubroutineIR(const SubroutineIR&) = delete;
    SubroutineIR& operator=(const SubroutineIR&) = delete;

};

// --- 3. Class Declaration (The Root) ---

struct ClassIR {
    std::string name;
    std::vector<VariableIR> classVariables; // STATIC and FIELD
    std::vector<SubroutineIR> subroutines;

    ClassIR(std::string n, std::vector<VariableIR> vars, std::vector<SubroutineIR> subs)
        : name(std::move(n)), classVariables(std::move(vars)), subroutines(std::move(subs)) {}
};

#endif // HIGH_LEVEL_IR_HPP