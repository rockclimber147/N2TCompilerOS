#include "JackCompiler/SymbolTable.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/IR/ExpressionIR.hpp"

class SemanticAnalyzer {
public:
    SemanticAnalyzer(ProjectSymbolTable& table) : symbolTable(table) {}

    void analyze(const std::vector<ClassIR>& projectASTs) {
        performSkeletonPass(projectASTs);
        performValidationPass(projectASTs);
    }

    void performSkeletonPass(const std::vector<ClassIR>& asts);
    void performValidationPass(const std::vector<ClassIR>& asts);

private:
    ProjectSymbolTable& symbolTable;
    void validateStatement(StatementIR* stmt, ClassEntry* cls, SubroutineEntry* sub);
    void validateExpression(ExpressionIR* expr, ClassEntry* cls, SubroutineEntry* sub);
    void validateVariable(VariableTermIR* var, ClassEntry* cls, SubroutineEntry* sub);
    void validateSubroutineCall(SubroutineCallIR* call, ClassEntry* cls, SubroutineEntry* sub);
};