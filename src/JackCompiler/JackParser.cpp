#include "JackCompiler/JackParser.hpp"
#include "JackCompiler/JackSpec.hpp"
#include "Tokenizer/TokenTypes.hpp"
#include "Tokenizer/Token.hpp"
#include "Tokenizer/TokenValidator.hpp"

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
    return "";
}

std::vector<VariableIR> JackParser::parseVarDec() {
    // This is very similar to parseClassVarDec, but always uses VarKind::VAR
    
    // 1. Consume 'var'
    
    // 2. Call parseType()
    
    // 3. Loop: Consume varName (IDENTIFIER) and optional ','
    
    // 4. Consume ';'
    
    return {}; // Placeholder
}

SubroutineIR JackParser::parseSubroutineDec() {
    // 1. Consume subroutine type keyword and determine SubroutineType

    // 2. Consume return type ('void' or call parseType())

    // 3. Consume subroutineName (IDENTIFIER)

    // 4. Consume '('
    // 5. Call parseParameterList(parameters) -> (Placeholder function)
    // 6. Consume ')'

    // 7. Call parseSubroutineBody(locals, statements) -> (Placeholder function)

    // 8. Assemble and return SubroutineIR

    return SubroutineIR(SubroutineType::FUNCTION, "void", ""); // Placeholder
}


// --- Remaining required Subroutine Detail Parsers (Stubs) ---

// void JackParser::parseParameterList(std::vector<VariableIR>& parameters) {
//     // Corresponds to 'parameterList': ((type varName) (',' type varName)*)?
    
//     // This will be implemented when we tackle arguments.
//     // Hint: The tokens will be VarKind::ARG
// }

// void JackParser::parseSubroutineBody(std::vector<VariableIR>& locals, 
//                                      std::vector<std::unique_ptr<StatementIR>>& statements) {
//     // Corresponds to 'subroutineBody': '{' varDec* statements '}'

//     // This will be implemented when we tackle statements.
// }

// All other parsing methods (statements, expressions, terms) will be added later.