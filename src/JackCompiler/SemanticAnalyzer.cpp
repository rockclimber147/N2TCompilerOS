#include "JackCompiler/SymbolTable.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/SemanticAnalyzer.hpp"
#include "JackCompiler/JackSpec.hpp"


void SemanticAnalyzer::performSkeletonPass(const std::vector<ClassIR>& asts) {
    for (const auto& classIR : asts) {
        symbolTable.addClass(classIR.name);
        ClassEntry* currentClass = symbolTable.getClass(classIR.name);

        for (const auto& var : classIR.classVariables) {
            currentClass->define(var.name, var.type, var.kind);
        }

        for (const auto& subIR : classIR.subroutines) {
            SubroutineEntry subEntry;
            subEntry.name = subIR.name;
            subEntry.returnType = subIR.returnType;
            subEntry.routineType = subIR.type;

            if (subIR.type == SubroutineType::METHOD) {
                subEntry.define(JackSpec::THIS, classIR.name, VarKind::ARG);
            }

            for (const auto& param : subIR.parameters) {
                subEntry.define(param.name, param.type, VarKind::ARG);
            }

            for (const auto& localVar : subIR.locals) {
                subEntry.define(localVar.name, localVar.type, VarKind::VAR);
            }

            currentClass->subroutines[subIR.name] = std::move(subEntry);
        }
    }
}

void SemanticAnalyzer::performValidationPass(const std::vector<ClassIR>& asts) {

}


void SemanticAnalyzer::validateStatement(StatementIR* stmt, ClassEntry* cls, SubroutineEntry* sub) {

}

void SemanticAnalyzer::validateExpression(ExpressionIR* expr, ClassEntry* cls, SubroutineEntry* sub) {

}

void SemanticAnalyzer::validateVariable(VariableTermIR* var, ClassEntry* cls, SubroutineEntry* sub) {

}

void SemanticAnalyzer::validateSubroutineCall(SubroutineCallIR* call, ClassEntry* cls, SubroutineEntry* sub) {

}
