#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "Tokenizer/TokenTypes.hpp"
#include "Tokenizer/Token.hpp"
#include "Tokenizer/TokenValidator.hpp"
#include "JackCompiler/IR/HighLevelIR.hpp"
#include "JackCompiler/IR/StatementIR.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

// --- I. Top-Level Declaration Parsers ---
ClassIR JackParser::parseClass() {

    validator_.expectSpecific(JackSpec::CLASS);
    Token classNameToken = validator_.expectType(TokenType::IDENTIFIER);
    validator_.expectSpecific(JackSpec::L_CURLY_BRACKET);

    std::vector<VariableIR> allClassVars;
    while (validator_.peekNext().lexeme == JackSpec::STATIC || 
           validator_.peekNext().lexeme == JackSpec::FIELD) {
        
        std::vector<VariableIR> vars = parseClassVarDec();
        allClassVars.insert(allClassVars.end(), vars.begin(), vars.end());
    }

    std::vector<SubroutineIR> subroutineDecs = {};
    std::unordered_set<std::string> subroutinePrefixes = {
        JackSpec::CONSTRUCTOR,
        JackSpec::FUNCTION, 
        JackSpec::METHOD
    };
    while (subroutinePrefixes.count(validator_.peekNext().lexeme) > 0) {
        subroutineDecs.push_back(parseSubroutineDec());
    }

    validator_.expectSpecific(JackSpec::R_CURLY_BRACKET);

    std::string className = classNameToken.lexeme;
    for (SubroutineIR& sub: subroutineDecs) {
        sub.fullyQualifiedName = className + "." + sub.name;
    }
    return ClassIR(classNameToken.lexeme, std::move(allClassVars), std::move(subroutineDecs));
}

std::vector<VariableIR> JackParser::parseClassVarDec() {
    std::vector<VariableIR> vars = {};

    Token kindToken = validator_.expectOneOfLexemes({JackSpec::STATIC, JackSpec::FIELD});
    VarKind kind = (kindToken.lexeme == JackSpec::STATIC) ? VarKind::STATIC : VarKind::FIELD;

    std::string varType = parseType();
    Token varName = validator_.expectType(TokenType::IDENTIFIER);
    
    vars.emplace_back(kind, varType, varName.lexeme);

    while (validator_.peekNext().lexeme == JackSpec::COMMA) {
        validator_.expectSpecific(JackSpec::COMMA);
        varName = validator_.expectType(TokenType::IDENTIFIER);
        vars.emplace_back(kind, varType, varName.lexeme);
    }

    validator_.expectSpecific(JackSpec::SEMICOLON);   
    return std::move(vars);
}

std::string JackParser::parseType() {
    Token t = validator_.peekNext();
    if (t.type == TokenType::KEYWORD) {
        Token typeToken = validator_.expectOneOfLexemes({
            JackSpec::INT, 
            JackSpec::CHAR, 
            JackSpec::BOOLEAN
        });
        return typeToken.lexeme;
    } 

    if (t.type == TokenType::IDENTIFIER) {
        Token identifierToken = validator_.expectType(TokenType::IDENTIFIER);
        return identifierToken.lexeme;
    }

    validator_.throwTokenError(t, "Expected a type (int, char, boolean, or class identifier), but found '" + 
                             t.lexeme + "'");
}

std::vector<VariableIR> JackParser::parseVarDec() {
    std::vector<VariableIR> vars = {};

    Token kindToken = validator_.expectSpecific(JackSpec::VAR);
    VarKind kind = VarKind::VAR;

    std::string varType = parseType();
    Token varName = validator_.expectType(TokenType::IDENTIFIER);
    
    vars.emplace_back(kind, varType, varName.lexeme);

    while (validator_.peekNext().lexeme == JackSpec::COMMA) {
        validator_.expectSpecific(JackSpec::COMMA);
        varName = validator_.expectType(TokenType::IDENTIFIER);
        vars.emplace_back(kind, varType, varName.lexeme);
    }

    validator_.expectSpecific(JackSpec::SEMICOLON);   
    return std::move(vars);
}

SubroutineIR JackParser::parseSubroutineDec() {
    Token subroutineKindToken = validator_.expectOneOfLexemes({
            JackSpec::CONSTRUCTOR, 
            JackSpec::FUNCTION, 
            JackSpec::METHOD
        });
    SubroutineType type;
    if (subroutineKindToken.lexeme == JackSpec::CONSTRUCTOR)  type = SubroutineType::CONSTRUCTOR;
    else if (subroutineKindToken.lexeme == JackSpec::FUNCTION)  type = SubroutineType::FUNCTION;
    else if (subroutineKindToken.lexeme == JackSpec::METHOD)  type = SubroutineType::METHOD;

    std::string subroutineReturnType;
    if (validator_.peekNext().lexeme == JackSpec::VOID) {
        subroutineReturnType = validator_.expectSpecific(JackSpec::VOID).lexeme;
    } else {
        subroutineReturnType = parseType();
    }

    Token subroutineIdentifierToken = validator_.expectType(TokenType::IDENTIFIER);
    SubroutineIR currentSubroutine(type, subroutineReturnType, subroutineIdentifierToken.lexeme);

    validator_.expectSpecific(JackSpec::L_PAREN);
    parseParameterList(currentSubroutine.parameters);
    validator_.expectSpecific(JackSpec::R_PAREN);

    parseSubroutineBody(currentSubroutine.locals, currentSubroutine.bodyStatements);

    return currentSubroutine;
}


// --- Remaining required Subroutine Detail Parsers (Stubs) ---

void JackParser::parseParameterList(std::vector<VariableIR>& parameters) {
    if (validator_.peekNext().lexeme == JackSpec::R_PAREN) {
        return;
    }
    std::string type = parseType();
    Token name = validator_.expectType(TokenType::IDENTIFIER);
    parameters.emplace_back(VarKind::ARG, type, name.lexeme);

    while (validator_.peekNext().lexeme == JackSpec::COMMA) {
        validator_.expectSpecific(JackSpec::COMMA);
        type = parseType();
        name = validator_.expectType(TokenType::IDENTIFIER);
        parameters.emplace_back(VarKind::ARG, type, name.lexeme);
    }
}

void JackParser::parseSubroutineBody(std::vector<VariableIR>& locals, 
                                     std::vector<std::unique_ptr<StatementIR>>& statements) {
    validator_.expectSpecific(JackSpec::L_CURLY_BRACKET);

    while (validator_.peekNext().lexeme == JackSpec::VAR) {
        std::vector<VariableIR> nextLine = parseVarDec();
        
        locals.insert(locals.end(), 
                      std::make_move_iterator(nextLine.begin()), 
                      std::make_move_iterator(nextLine.end()));
    }
    statements = parseStatements();
    validator_.expectSpecific(JackSpec::R_CURLY_BRACKET);
}

std::vector<std::unique_ptr<StatementIR>> JackParser::parseStatements() {
    std::vector<std::unique_ptr<StatementIR>> statementList;
    
    static const std::unordered_set<std::string> statementKeywords = {
        JackSpec::LET, JackSpec::IF, JackSpec::WHILE, 
        JackSpec::DO, JackSpec::RETURN
    };

    while (statementKeywords.count(validator_.peekNext().lexeme) > 0) {
        statementList.push_back(parseStatement());
    }

    return statementList; 
}

std::unique_ptr<StatementIR> JackParser::parseStatement() {
    Token next = validator_.peekNext();

    if (next.lexeme == JackSpec::LET)    return parseLetStatement();
    if (next.lexeme == JackSpec::IF)     return parseIfStatement();
    if (next.lexeme == JackSpec::WHILE)  return parseWhileStatement();
    if (next.lexeme == JackSpec::DO)     return parseDoStatement();
    if (next.lexeme == JackSpec::RETURN) return parseReturnStatement();

    validator_.throwTokenError(next, "Expected a statement (let, if, while, do, or return)");
    return nullptr; 
}   

// --- Specific Statement Stubs ---

std::unique_ptr<StatementIR> JackParser::parseLetStatement() {
    std::unique_ptr<ExpressionIR> indexExpression = nullptr;
    validator_.expectSpecific(JackSpec::LET);

    std::string varName = validator_.expectType(TokenType::IDENTIFIER).lexeme;

    if (validator_.peekNext().lexeme == JackSpec::L_SQUARE_BRACKET) {
        validator_.expectSpecific(JackSpec::L_SQUARE_BRACKET);
        indexExpression = nullptr;
        validator_.expectSpecific(JackSpec::R_SQUARE_BRACKET);
    }

    validator_.expectSpecific(JackSpec::EQ);

    std::unique_ptr<ExpressionIR> assignmentExpression = nullptr;

    validator_.expectSpecific(JackSpec::SEMICOLON);

    return std::make_unique<LetStatementIR>(varName, std::move(indexExpression), std::move(assignmentExpression));
}

std::unique_ptr<StatementIR> JackParser::parseIfStatement() {
    validator_.expectSpecific(JackSpec::IF);
    validator_.expectSpecific(JackSpec::L_PAREN);
    std::unique_ptr<ExpressionIR> condition = nullptr;
    validator_.expectSpecific(JackSpec::R_PAREN);

    validator_.expectSpecific(JackSpec::L_CURLY_BRACKET);
    std::vector<std::unique_ptr<StatementIR>> ifBlock = parseStatements();
    validator_.expectSpecific(JackSpec::R_CURLY_BRACKET);

    std::vector<std::unique_ptr<StatementIR>> elseBlock = {};
    if (validator_.peekNext().lexeme == JackSpec::ELSE) {
        validator_.expectSpecific(JackSpec::ELSE);
        validator_.expectSpecific(JackSpec::L_CURLY_BRACKET);
        elseBlock = parseStatements();
        validator_.expectSpecific(JackSpec::R_CURLY_BRACKET);
    }

    return std::make_unique<IfStatementIR>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
}

std::unique_ptr<StatementIR> JackParser::parseWhileStatement() {
    validator_.expectSpecific(JackSpec::WHILE);
    validator_.expectSpecific(JackSpec::L_PAREN);
    std::unique_ptr<ExpressionIR> condition = nullptr;
    validator_.expectSpecific(JackSpec::R_PAREN);

    validator_.expectSpecific(JackSpec::L_CURLY_BRACKET);
    std::vector<std::unique_ptr<StatementIR>> bodyStatements = parseStatements();
    validator_.expectSpecific(JackSpec::R_CURLY_BRACKET);


    return std::make_unique<WhileStatementIR>(std::move(condition), std::move(bodyStatements));
}

std::unique_ptr<StatementIR> JackParser::parseDoStatement() {
    validator_.expectSpecific(JackSpec::DO);
    std::unique_ptr<ExpressionIR> expression = nullptr;
    validator_.expectSpecific(JackSpec::SEMICOLON);
    return std::make_unique<DoStatementIR>(std::move(expression));
}

std::unique_ptr<StatementIR> JackParser::parseReturnStatement() {
    validator_.expectSpecific(JackSpec::RETURN);
    std::unique_ptr<ExpressionIR> expression = nullptr;
    validator_.expectSpecific(JackSpec::SEMICOLON);
    return std::make_unique<ReturnStatementIR>(std::move(expression));
}