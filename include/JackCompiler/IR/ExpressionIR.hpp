#ifndef EXPRESSION_IR_HPP
#define EXPRESSION_IR_HPP

#include <string>
#include <vector>
#include <memory>

struct ExpressionIR {
    virtual ~ExpressionIR() = default;
};

// Represents: term (op term)*
struct BinaryExpressionIR : public ExpressionIR {
    std::unique_ptr<ExpressionIR> leftTerm;
    std::string op;
    std::unique_ptr<ExpressionIR> rightTerm;

    BinaryExpressionIR(std::unique_ptr<ExpressionIR> left, std::string operation, std::unique_ptr<ExpressionIR> right)
        : leftTerm(std::move(left)), op(std::move(operation)), rightTerm(std::move(right)) {}
};

struct IntegerLiteralIR : public ExpressionIR {
    int value;
    IntegerLiteralIR(int val) : value(val) {}
};

struct StringLiteralIR : public ExpressionIR {
    std::string value;
    StringLiteralIR(std::string val) : value(std::move(val)) {}
};

struct KeywordLiteralIR : public ExpressionIR {
    std::string keyword;
    KeywordLiteralIR(std::string kw) : keyword(std::move(kw)) {}
};

struct VariableTermIR : public ExpressionIR {
    std::string name;
    std::unique_ptr<ExpressionIR> arrayIndex;
    VariableTermIR(std::string n, std::unique_ptr<ExpressionIR> idx = nullptr) 
        : name(std::move(n)), arrayIndex(std::move(idx)) {}
};

struct UnaryTermIR : public ExpressionIR {
    std::string op;
    std::unique_ptr<ExpressionIR> term;
    UnaryTermIR(std::string operation, std::unique_ptr<ExpressionIR> t) 
        : op(std::move(operation)), term(std::move(t)) {}
};

struct SubroutineCallIR : public ExpressionIR {
    std::string target;
    std::string methodName;
    std::vector<std::unique_ptr<ExpressionIR>> arguments;

    SubroutineCallIR(std::string tgt, std::string method, std::vector<std::unique_ptr<ExpressionIR>> args)
        : target(std::move(tgt)), methodName(std::move(method)), arguments(std::move(args)) {}
};

#endif