#ifndef JACK_PARSER_HPP
#define JACK_PARSER_HPP

#include "Tokenizer/TokenValidator.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"

struct StatementIR;
struct ExpressionIR;

class JackParser {
public:
    JackParser(TokenValidator& validator) : validator_(validator) {}
    ClassIR parseClass();

private:
    TokenValidator& validator_;
    std::vector<VariableIR> parseClassVarDec();
    SubroutineIR parseSubroutineDec();
    void parseParameterList(std::vector<VariableIR>& parameters);
    void parseSubroutineBody(std::vector<VariableIR>& locals, 
                             std::vector<std::unique_ptr<StatementIR>>& statements);
    std::vector<VariableIR> parseVarDec();
    std::vector<std::unique_ptr<StatementIR>> parseStatements();
    std::unique_ptr<StatementIR> parseStatement();
    std::unique_ptr<StatementIR> parseLetStatement();
    std::unique_ptr<StatementIR> parseIfStatement();
    std::unique_ptr<StatementIR> parseWhileStatement();
    std::unique_ptr<StatementIR> parseDoStatement();
    std::unique_ptr<StatementIR> parseReturnStatement();
    std::unique_ptr<ExpressionIR> parseExpression();
    std::unique_ptr<ExpressionIR> parseTerm();
    std::unique_ptr<ExpressionIR> parseSubroutineCall(const Token& firstTokenLexeme);
    std::vector<std::unique_ptr<ExpressionIR>> parseExpressionList();
    std::string parseType();
};

#endif // JACK_PARSER_HPP