#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/IR/StatementIR.hpp"

SubroutineIR::SubroutineIR(SubroutineType t, std::string retType, std::string n)
    : type(t), returnType(std::move(retType)), name(std::move(n)) {}

SubroutineIR::~SubroutineIR() = default; 

SubroutineIR::SubroutineIR(SubroutineIR&&) noexcept = default;
SubroutineIR& SubroutineIR::operator=(SubroutineIR&&) noexcept = default;