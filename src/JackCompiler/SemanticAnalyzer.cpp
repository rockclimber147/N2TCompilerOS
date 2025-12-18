#include "JackCompiler/SymbolTable.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/SemanticAnalyzer.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "JackCompiler/IR/StatementIR.hpp"


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
    for (const auto& classIR : asts) {
        ClassEntry* cls = symbolTable.getClass(classIR.name);
        if (!cls) continue;

        for (const auto& subroutineIR : classIR.subroutines) {
            SubroutineEntry* sub = &cls->subroutines.at(subroutineIR.name);

            for (const auto& statement : subroutineIR.bodyStatements) {
                validateStatement(statement.get(), cls, sub);
            }
        }
    }
}


void SemanticAnalyzer::validateStatement(StatementIR* stmt, ClassEntry* cls, SubroutineEntry* sub) {
    if (!stmt) return;

    if (auto letStmt = dynamic_cast<LetStatementIR*>(stmt)) {
        const VariableSymbol* var = sub->lookup(letStmt->varName, cls);
        if (!var) {
            throw std::runtime_error("Undeclared variable: " + letStmt->varName);
        }

        if (letStmt->indexExpression) {
            validateExpression(letStmt->indexExpression.get(), cls, sub);
        }

        validateExpression(letStmt->assignedExpression.get(), cls, sub);
    } 
    
    else if (auto ifStmt = dynamic_cast<IfStatementIR*>(stmt)) {
        validateExpression(ifStmt->condition.get(), cls, sub);
        
        for (auto& s : ifStmt->ifBlock) {
            validateStatement(s.get(), cls, sub);
        }
        
        for (auto& s : ifStmt->elseBlock) {
            validateStatement(s.get(), cls, sub);
        }
    }

    else if (auto whileStmt = dynamic_cast<WhileStatementIR*>(stmt)) {
        validateExpression(whileStmt->condition.get(), cls, sub);
        
        for (auto& s : whileStmt->body) {
            validateStatement(s.get(), cls, sub);
        }
    }

    else if (auto doStmt = dynamic_cast<DoStatementIR*>(stmt)) {
        validateExpression(doStmt->subroutineCall.get(), cls, sub);
    }

    else if (auto retStmt = dynamic_cast<ReturnStatementIR*>(stmt)) {
        if (retStmt->expression) {
            validateExpression(retStmt->expression.get(), cls, sub);
        }
    }
}

void SemanticAnalyzer::validateExpression(ExpressionIR* expr, ClassEntry* cls, SubroutineEntry* sub) {
    if (!expr) return;

    if (auto varTerm = dynamic_cast<VariableTermIR*>(expr)) {
        if (varTerm->name == "this") {
            if (sub->routineType == SubroutineType::FUNCTION) {
                throw std::runtime_error("Line " + std::to_string(varTerm->line) + ": 'this' used in static function.");
            }
            return;
        }

        const VariableSymbol* symbol = sub->lookup(varTerm->name, cls);
        if (!symbol) {
            throw std::runtime_error("Line " + std::to_string(varTerm->line) + ": Undeclared variable '" + varTerm->name + "'");
        }

        if (sub->routineType == SubroutineType::FUNCTION && symbol->kind == VarKind::FIELD) {
            throw std::runtime_error("Line " + std::to_string(varTerm->line) + ": Cannot access field '" + varTerm->name + "' in static function.");
        }

        if (varTerm->arrayIndex) {
            validateExpression(varTerm->arrayIndex.get(), cls, sub);
        }
    }

    else if (auto binOp = dynamic_cast<BinaryExpressionIR*>(expr)) {
        validateExpression(binOp->leftTerm.get(), cls, sub);
        validateExpression(binOp->rightTerm.get(), cls, sub);
    }

    else if (auto unary = dynamic_cast<UnaryTermIR*>(expr)) {
        validateExpression(unary->term.get(), cls, sub);
    }

    else if (auto call = dynamic_cast<SubroutineCallIR*>(expr)) {
        validateSubroutineCall(call, cls, sub);
    }
}

void SemanticAnalyzer::validateVariable(VariableTermIR* var, ClassEntry* cls, SubroutineEntry* sub) {
    if (var->name == "this") {
        if (sub->routineType == SubroutineType::FUNCTION) {
            throw std::runtime_error("Line " + std::to_string(var->line) + 
                ": Cannot use keyword 'this' in a static function.");
        }
        return; 
    }

    const VariableSymbol* symbol = sub->lookup(var->name, cls);
    if (!symbol) {
        throw std::runtime_error("Line " + std::to_string(var->line) + 
            ": Undeclared variable '" + var->name + "'");
    }

    if (sub->routineType == SubroutineType::FUNCTION && symbol->kind == VarKind::FIELD) {
        throw std::runtime_error("Line " + std::to_string(var->line) + 
            ": Cannot access field '" + var->name + "' from a static function context.");
    }

    if (var->arrayIndex) {
        validateExpression(var->arrayIndex.get(), cls, sub);
    }
}

void SemanticAnalyzer::validateSubroutineCall(SubroutineCallIR* call, ClassEntry* cls, SubroutineEntry* sub) {
    std::string targetClassName;
    bool isMethodCall = false;

    if (call->target == JackSpec::THIS) {
        if (sub->routineType == SubroutineType::FUNCTION) {
            throw std::runtime_error("Line " + std::to_string(call->line) + 
                ": Cannot call method '" + call->methodName + "' from a static function.");
        }
        targetClassName = cls->className;
        isMethodCall = true;
    } 
    else {
        const VariableSymbol* var = sub->lookup(call->target, cls);
        if (var) {
            targetClassName = var->type; 
            isMethodCall = true;
        } 

        else if (symbolTable.classExists(call->target)) {
            targetClassName = call->target;
            isMethodCall = false; 
        } 
        else {
            throw std::runtime_error("Line " + std::to_string(call->line) + 
                ": Unknown identifier '" + call->target + "'");
        }
    }

    ClassEntry* targetClass = symbolTable.getClass(targetClassName);
    if (!targetClass) {
        throw std::runtime_error("Line " + std::to_string(call->line) + 
            ": Class '" + targetClassName + "' not found.");
    }

    auto it = targetClass->subroutines.find(call->methodName);
    if (it == targetClass->subroutines.end()) {
        throw std::runtime_error("Line " + std::to_string(call->line) + 
            ": Class '" + targetClassName + "' has no subroutine '" + call->methodName + "'");
    }

    const SubroutineEntry& targetSub = it->second;
    if (isMethodCall && targetSub.routineType == SubroutineType::FUNCTION) {
    }
    if (!isMethodCall && targetSub.routineType == SubroutineType::METHOD) {
        throw std::runtime_error("Line " + std::to_string(call->line) + 
            ": Cannot call method '" + call->methodName + "' without an instance.");
    }

    for (auto& arg : call->arguments) {
        validateExpression(arg.get(), cls, sub);
    }
}