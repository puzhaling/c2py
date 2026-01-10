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

    const std::vector<Token>& getTokens() const;
    size_t getPos() const;
    const Token& peek () const;
    const Token& previous() const;
    const Token& advance();
    bool atEnd() const;
    bool check(TokenType type, const std::string& lexeme = "") const;
    bool match(TokenType type, const std::string& lexeme = "");
    void expect(TokenType type, const std::string& lexeme = "");
    std::string tokenLocation() const;
};

#endif  /*  ! PARSER_TESTER_H  */