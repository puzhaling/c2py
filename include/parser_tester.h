#ifndef PARSER_TESTER_H
# define PARSER_TESTER_H

#include    "parser.h"

class ParserTester {
public:
    Parser& p;

    explicit ParserTester (Parser &p)
        :   p(p)
    {
    }

    std::vector<Token> getTokens() const;
    size_t getPos() const;
    void setPos(size_t pos);
    SymbolTable& getSymbolTable();

    const Token& peek () const;
    const Token& previous() const;
    const Token& advance();
    bool atEnd() const;
    bool check(TokenType type, const std::string& lexeme = "") const;
    bool match(TokenType type, const std::string& lexeme = "");
    void expect(TokenType type, const std::string& lexeme = "");
    std::string tokenLocation() const;

    ProgramPtr parseProgram();
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
};

#endif  /*  ! PARSER_TESTER_H  */