#pragma once
#include "token.h"
#include "ast.h"
#include "symbol_table.h"
#include <vector>
#include <memory>
#include <string>


class ParserTester;

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // Разобрать программу; бросает std::runtime_error при синтаксической ошибке.
    ProgramPtr parseProgram();

    friend class ParserTester;
    
private:
    const std::vector<Token> tokens;
    size_t pos;
    SymbolTable symbols;

    // Навигация по токенам
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool atEnd() const;
    bool check(TokenType type, const std::string& lexeme = "") const;
    bool match(TokenType type, const std::string& lexeme = "");
    void expect(TokenType type, const std::string& lexeme = "");

    // Разборные функции (recursive descent)
    FuncPtr parseFunction();
    std::unique_ptr<BlockStmt> parseBlock();

    StmtPtr parseStatement();
    StmtPtr parseIf();
    StmtPtr parseWhile();
    StmtPtr parseDoWhile();
    StmtPtr parseFor();
    StmtPtr parseReturn();
    StmtPtr parseBreak();
    StmtPtr parseContinue();
    StmtPtr parseVarDeclStatement();

    ExprPtr parseExpression();
    ExprPtr parseAssignment();
    ExprPtr parseLogicalOr();
    ExprPtr parseLogicalAnd();
    ExprPtr parseEquality();
    ExprPtr parseRelational();
    ExprPtr parseAdditive();
    ExprPtr parseMultiplicative();
    ExprPtr parseUnary();
    ExprPtr parsePostfix();
    ExprPtr parsePrimary();

    // Утилиты
    std::string tokenLocation() const;
};
