#ifndef STATEMENT_IR_HPP
#define STATEMENT_IR_HPP

#include <string>
#include <vector>
#include <memory>

struct ExpressionIR;

struct StatementIR {
    virtual ~StatementIR() = default;
};

struct LetStatementIR : public StatementIR {
    std::string varName;
    std::unique_ptr<ExpressionIR> indexExpression;
    std::unique_ptr<ExpressionIR> assignedExpression;

    LetStatementIR(std::string name, 
                   std::unique_ptr<ExpressionIR> idx, 
                   std::unique_ptr<ExpressionIR> expr)
        : varName(std::move(name)), indexExpression(std::move(idx)), assignedExpression(std::move(expr)) {}
};

struct IfStatementIR : public StatementIR {
    std::unique_ptr<ExpressionIR> condition;
    std::vector<std::unique_ptr<StatementIR>> ifBlock;
    std::vector<std::unique_ptr<StatementIR>> elseBlock;

    IfStatementIR(std::unique_ptr<ExpressionIR> cond, 
                  std::vector<std::unique_ptr<StatementIR>> ifB, 
                  std::vector<std::unique_ptr<StatementIR>> elseB)
        : condition(std::move(cond)), ifBlock(std::move(ifB)), elseBlock(std::move(elseB)) {}
};

struct WhileStatementIR : public StatementIR {
    std::unique_ptr<ExpressionIR> condition;
    std::vector<std::unique_ptr<StatementIR>> body;

    WhileStatementIR(std::unique_ptr<ExpressionIR> cond, 
                     std::vector<std::unique_ptr<StatementIR>> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct DoStatementIR : public StatementIR {
    std::unique_ptr<ExpressionIR> subroutineCall;

    DoStatementIR(std::unique_ptr<ExpressionIR> call) 
        : subroutineCall(std::move(call)) {}
};

struct ReturnStatementIR : public StatementIR {
    std::unique_ptr<ExpressionIR> expression;

    ReturnStatementIR(std::unique_ptr<ExpressionIR> expr = nullptr) 
        : expression(std::move(expr)) {}
};

#endif