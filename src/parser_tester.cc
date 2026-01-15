#include    "parser_tester.h"


std::vector<Token>
ParserTester::getTokens () const
{
    return p.tokens;
}

size_t
ParserTester::getPos () const
{
    return p.pos;
}

void
ParserTester::setPos (size_t pos)
{
    p.pos = pos;
}

SymbolTable&
ParserTester::getSymbolTable ()
{
    return p.symbols;
}

const Token& 
ParserTester::peek () const
{
    return p.peek ();
}

const Token&
ParserTester::previous () const
{
    return p.previous ();
}

const Token&
ParserTester::advance ()
{
    return p.advance ();
}

bool
ParserTester::atEnd () const
{
    return p.atEnd ();
}

bool
ParserTester::check (TokenType type, const std::string& lexeme) const
{
    return p.check (type, lexeme);
}

bool
ParserTester::match (TokenType type, const std::string& lexeme)
{
    return p.match (type, lexeme);
}

void
ParserTester::expect (TokenType type, const std::string& lexeme)
{
    return p.expect (type, lexeme);
}

std::string
ParserTester::tokenLocation () const
{
    return p.tokenLocation ();
}

ProgramPtr
ParserTester::parseProgram ()
{
    return p.parseProgram ();
}

FuncPtr
ParserTester::parseFunction ()
{
    return p.parseFunction ();
}

std::unique_ptr<BlockStmt>
ParserTester::parseBlock ()
{
    return p.parseBlock ();
}

StmtPtr
ParserTester::parseStatement ()
{
    return p.parseStatement ();
}

StmtPtr
ParserTester::parseIf ()
{
    return p.parseIf ();
}

StmtPtr
ParserTester::parseWhile ()
{
    return p.parseWhile ();
}

StmtPtr
ParserTester::parseDoWhile ()
{
    return p.parseDoWhile ();
}

StmtPtr
ParserTester::parseFor ()
{
    return p.parseFor ();
}

StmtPtr
ParserTester::parseReturn ()
{
    return p.parseReturn ();
}

StmtPtr
ParserTester::parseBreak ()
{
    return p.parseBreak ();
}

StmtPtr
ParserTester::parseContinue ()
{
    return p.parseContinue ();
}

StmtPtr
ParserTester::parseVarDeclStatement ()
{
    return p.parseVarDeclStatement ();
}

ExprPtr
ParserTester::parseExpression ()
{
    return p.parseExpression ();
}

ExprPtr
ParserTester::parseAssignment ()
{
    return p.parseAssignment ();
}

ExprPtr
ParserTester::parseLogicalOr ()
{
    return p.parseLogicalOr ();
}

ExprPtr
ParserTester::parseLogicalAnd ()
{
    return p.parseLogicalAnd ();
}

ExprPtr
ParserTester::parseEquality ()
{
    return p.parseEquality ();
}

ExprPtr
ParserTester::parseRelational ()
{
    return p.parseRelational ();
}

ExprPtr
ParserTester::parseAdditive ()
{
    return p.parseAdditive ();
}

ExprPtr
ParserTester::parseMultiplicative ()
{
    return p.parseMultiplicative ();
}

ExprPtr
ParserTester::parseUnary ()
{
    return p.parseUnary ();
}

ExprPtr
ParserTester::parsePostfix ()
{
    return p.parsePostfix ();
}

ExprPtr
ParserTester::parsePrimary ()
{
    return p.parsePrimary ();
}

