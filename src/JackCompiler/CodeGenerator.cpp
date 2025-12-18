#include "JackCompiler/CodeGenerator.hpp"
#include "JackCompiler/JackSpec.hpp"

void CodeGenerator::writeClass(const ClassIR& classIR) {
    const ClassEntry* cls = symbolTable.getClass(classIR.name);
    for (const auto& subIR : classIR.subroutines) {
        writeSubroutine(subIR, cls);
    }
}

void CodeGenerator::writeSubroutine(const SubroutineIR& subIR, const ClassEntry* cls) {
    const SubroutineEntry* sub = &cls->subroutines.at(subIR.name);
    
    vmWriter.writeFunction(cls->className + "." + subIR.name, sub->varCounter);

    if (subIR.type == SubroutineType::CONSTRUCTOR) {
        vmWriter.writePush(Segment::CONST, cls->fieldCounter);
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(Segment::POINTER, 0);
    } 
    else if (subIR.type == SubroutineType::METHOD) {
        vmWriter.writePush(Segment::ARG, 0);
        vmWriter.writePop(Segment::POINTER, 0);
    }

    for (const auto& stmt : subIR.bodyStatements) {
        writeStatement(stmt.get(), cls, sub);
    }
}

void CodeGenerator::writeExpression(const ExpressionIR* expr, const ClassEntry* cls, const SubroutineEntry* sub) {
    if (!expr) return;

    if (auto intLit = dynamic_cast<const IntegerLiteralIR*>(expr)) {
        vmWriter.writePush(Segment::CONST, intLit->value);
    }

    else if (auto keyLit = dynamic_cast<const KeywordLiteralIR*>(expr)) {
        if (keyLit->keyword == JackSpec::TRUE) {
            vmWriter.writePush(Segment::CONST, 0);
            vmWriter.writeArithmetic("not"); // true is -1 in Jack
        } else if (keyLit->keyword == JackSpec::FALSE || keyLit->keyword == JackSpec::NULL_KEYWORD) {
            vmWriter.writePush(Segment::CONST, 0);
        } else if (keyLit->keyword == JackSpec::THIS) {
            vmWriter.writePush(Segment::POINTER, 0);
        }
    }

    else if (auto varTerm = dynamic_cast<const VariableTermIR*>(expr)) {
        const VariableSymbol* sym = sub->lookup(varTerm->name, cls);
        vmWriter.writePush(mapKindToSegment(sym->kind), sym->index);
        
        if (varTerm->arrayIndex) {
            writeExpression(varTerm->arrayIndex.get(), cls, sub);
            vmWriter.writeArithmetic("add");
            vmWriter.writePop(Segment::POINTER, 1);
            vmWriter.writePush(Segment::THAT, 0);
        }
    }

    else if (auto binOp = dynamic_cast<const BinaryExpressionIR*>(expr)) {
        writeExpression(binOp->leftTerm.get(), cls, sub);
        writeExpression(binOp->rightTerm.get(), cls, sub);

        if (binOp->op == "+") vmWriter.writeArithmetic("add");
        else if (binOp->op == "-") vmWriter.writeArithmetic("sub");
        else if (binOp->op == "*") vmWriter.writeCall("Math.multiply", 2);
        else if (binOp->op == "/") vmWriter.writeCall("Math.divide", 2);
        else if (binOp->op == ">") vmWriter.writeArithmetic("gt");
        else if (binOp->op == "<") vmWriter.writeArithmetic("lt");
        else if (binOp->op == "=") vmWriter.writeArithmetic("eq");
        else if (binOp->op == "&") vmWriter.writeArithmetic("and");
        else if (binOp->op == "|") vmWriter.writeArithmetic("or");
    }

    // 5. Subroutine Calls
    else if (auto call = dynamic_cast<const SubroutineCallIR*>(expr)) {
        writeSubroutineCall(call, cls, sub);
    }
}

void CodeGenerator::writeStatement(const StatementIR* stmt, const ClassEntry* cls, const SubroutineEntry* sub) {
    if (!stmt) return;

    if (auto letStmt = dynamic_cast<const LetStatementIR*>(stmt)) {
        if (letStmt->indexExpression) {
            const VariableSymbol* var = sub->lookup(letStmt->varName, cls);
            vmWriter.writePush(mapKindToSegment(var->kind), var->index);
            writeExpression(letStmt->indexExpression.get(), cls, sub);
            vmWriter.writeArithmetic("add");

            writeExpression(letStmt->assignedExpression.get(), cls, sub);
            
            vmWriter.writePop(Segment::TEMP, 0);
            vmWriter.writePop(Segment::POINTER, 1);
            vmWriter.writePush(Segment::TEMP, 0);
            vmWriter.writePop(Segment::THAT, 0);
        } else {
            writeExpression(letStmt->assignedExpression.get(), cls, sub);
            const VariableSymbol* var = sub->lookup(letStmt->varName, cls);
            vmWriter.writePop(mapKindToSegment(var->kind), var->index);
        }
    }

    else if (auto ifStmt = dynamic_cast<const IfStatementIR*>(stmt)) {
        std::string trueLabel = generateLabel("IF_TRUE");
        std::string falseLabel = generateLabel("IF_FALSE");
        std::string endLabel = generateLabel("IF_END");

        writeExpression(ifStmt->condition.get(), cls, sub);
        vmWriter.writeIf(trueLabel);
        vmWriter.writeGoTo(falseLabel);

        vmWriter.writeLabel(trueLabel);
        for (auto& s : ifStmt->ifBlock) writeStatement(s.get(), cls, sub);
        
        if (!ifStmt->elseBlock.empty()) {
            vmWriter.writeGoTo(endLabel);
            vmWriter.writeLabel(falseLabel);
            for (auto& s : ifStmt->elseBlock) writeStatement(s.get(), cls, sub);
            vmWriter.writeLabel(endLabel);
        } else {
            vmWriter.writeLabel(falseLabel);
        }
    }

    else if (auto whileStmt = dynamic_cast<const WhileStatementIR*>(stmt)) {
        std::string continueLabel = generateLabel("WHILE_EXP");
        std::string endLabel = generateLabel("WHILE_END");

        vmWriter.writeLabel(continueLabel);
        writeExpression(whileStmt->condition.get(), cls, sub);
        vmWriter.writeArithmetic("not");
        vmWriter.writeIf(endLabel);

        for (auto& s : whileStmt->body) writeStatement(s.get(), cls, sub);
        
        vmWriter.writeGoTo(continueLabel);
        vmWriter.writeLabel(endLabel);
    }

    else if (auto doStmt = dynamic_cast<const DoStatementIR*>(stmt)) {
        writeExpression(doStmt->subroutineCall.get(), cls, sub);
        vmWriter.writePop(Segment::TEMP, 0); 
    }

    else if (auto retStmt = dynamic_cast<const ReturnStatementIR*>(stmt)) {
        if (retStmt->expression) {
            writeExpression(retStmt->expression.get(), cls, sub);
        } else {
            vmWriter.writePush(Segment::CONST, 0); 
        }
        vmWriter.writeReturn();
    }
}

void CodeGenerator::writeVariable(const VariableTermIR* var, const ClassEntry* cls, const SubroutineEntry* sub) {

    const VariableSymbol* sym = sub->lookup(var->name, cls);
    vmWriter.writePush(mapKindToSegment(sym->kind), sym->index);

    if (var->arrayIndex) {
        writeExpression(var->arrayIndex.get(), cls, sub);
        vmWriter.writeArithmetic("add");
        vmWriter.writePop(Segment::POINTER, 1);
        vmWriter.writePush(Segment::THAT, 0);
    }
}

void CodeGenerator::writeSubroutineCall(const SubroutineCallIR* call, const ClassEntry* cls, const SubroutineEntry* sub) {
    std::string targetClassName;
    int argCount = call->arguments.size();

    if (call->target == "this") {
        vmWriter.writePush(Segment::POINTER, 0);
        targetClassName = cls->className;
        argCount++;
    } 
    else {
        const VariableSymbol* var = sub->lookup(call->target, cls);
        
        if (var) {
            vmWriter.writePush(mapKindToSegment(var->kind), var->index);
            targetClassName = var->type;
            argCount++;
        } else {
            targetClassName = call->target;
        }
    }

    for (const auto& arg : call->arguments) {
        writeExpression(arg.get(), cls, sub);
    }
    vmWriter.writeCall(targetClassName + "." + call->methodName, argCount);
}

std::string CodeGenerator::generateLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter++);
}

Segment CodeGenerator::mapKindToSegment(VarKind kind) {
    switch (kind) {
        case VarKind::STATIC: return Segment::STATIC;
        case VarKind::FIELD:  return Segment::THIS;
        case VarKind::ARG:    return Segment::ARG;
        case VarKind::VAR:    return Segment::LOCAL;
    }
    return Segment::TEMP;
}