#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <vector>
#include <string>
#include <memory>

#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/IR/StatementIR.hpp"
#include "JackCompiler/IR/ExpressionIR.hpp"
#include "JackCompiler/SymbolTable.hpp"
#include "JackCompiler/VMWriter.hpp"

class CodeGenerator {
public:
    CodeGenerator(ProjectSymbolTable& table, VMWriter& writer) 
        : symbolTable(table), vmWriter(writer), labelCounter(0) {}

    void writeClass(const ClassIR& classIR);

private:
    ProjectSymbolTable& symbolTable;
    VMWriter& vmWriter;
    int labelCounter;
    void writeSubroutine(const SubroutineIR& subIR, const ClassEntry* cls);
    
    void writeStatement(const StatementIR* stmt, const ClassEntry* cls, const SubroutineEntry* sub);
    void writeExpression(const ExpressionIR* expr, const ClassEntry* cls, const SubroutineEntry* sub);

    void writeVariable(const VariableTermIR* var, const ClassEntry* cls, const SubroutineEntry* sub);
    void writeSubroutineCall(const SubroutineCallIR* call, const ClassEntry* cls, const SubroutineEntry* sub);

    std::string generateLabel(const std::string& prefix);
    Segment mapKindToSegment(VarKind kind);
};

#endif