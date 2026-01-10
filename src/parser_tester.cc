#include    "parser_tester.h"


const std::vector<Token>&
ParserTester::getTokens() const
{
    return p.tokens;
}

size_t
ParserTester::getPos() const
{
    return p.pos;
}

const Token& 
ParserTester::peek() const
{
    return p.peek();
}

const Token&
ParserTester::previous() const
{
    return p.previous();
}

const Token&
ParserTester::advance()
{
    return p.advance();
}

bool
ParserTester::atEnd () const
{
    return p.atEnd();
}

bool
ParserTester::check(TokenType type, const std::string& lexeme) const
{
    return p.check(type, lexeme);
}

bool
ParserTester::match(TokenType type, const std::string& lexeme)
{
    return p.match(type, lexeme);
}

void
ParserTester::expect(TokenType type, const std::string& lexeme)
{
    return p.expect(type, lexeme);
}

std::string
ParserTester::tokenLocation() const
{
    return p.tokenLocation();
}