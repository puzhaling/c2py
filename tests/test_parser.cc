#include    "lexer.h"
#include    "parser.h"
#include    "parser_tester.h"

#include    <iostream>
#include    <CppUTest/TestHarness.h>


static bool
tokens_eq (const Token &fst, const Token &snd)
{
    return fst.type == snd.type && fst.lexeme == snd.lexeme;
}


TEST_GROUP (parser_test_group)
{
};

TEST (parser_test_group, test_at_end)
{
    Lexer l("");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    CHECK (pt.atEnd() == true);
}

TEST (parser_test_group, test_peek)
{
    Lexer l("1 2 3");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    CHECK ( tokens_eq(pt.peek(),tokens[0]) );
}

TEST (parser_test_group, test_advance)
{
    Lexer l("1 2 3 4 5");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    CHECK ( tokens_eq(pt.advance(),tokens[0]) );
    CHECK ( tokens_eq(pt.advance(),tokens[1]) );
    CHECK ( tokens_eq(pt.advance(),tokens[2]) );
    CHECK ( tokens_eq(pt.advance(),tokens[3]) );
    CHECK ( tokens_eq(pt.advance(),tokens[4]) );
    CHECK ( tokens_eq(pt.advance(),tokens[4]) );
    CHECK ( tokens_eq(pt.advance(),tokens[4]) );
    CHECK ( tokens_eq(pt.advance(),tokens[4]) );
    CHECK ( tokens_eq(pt.advance(),tokens[4]) );
}

TEST (parser_test_group, test_previous)
{
    Lexer l("1 2 3 4 5");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    CHECK ( tokens_eq(pt.previous(),tokens[0]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[0]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[1]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[2]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[3]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[4]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[4]) );
    pt.advance();
    CHECK ( tokens_eq(pt.previous(),tokens[4]) );
    pt.advance();
}

TEST (parser_test_group, test_check)
{
    Lexer l("1 2 3 4 5");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    // Be aware that Parser::check returns FALSE
    CHECK ( ! pt.check(TokenType::Unknown, ""));

    tokens[0].type = TokenType::Unknown;
    CHECK ( ! pt.check(TokenType::Number, "1"));
    tokens[0].type = TokenType::Number;

    CHECK (pt.check(TokenType::Number, "1"));

    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();
    pt.advance();

    CHECK ( ! pt.check(TokenType::EndOfFile, ""));
}

TEST (parser_test_group, test_match)
{
    Lexer l("1 2 3 4 5");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    CHECK (pt.match(TokenType::Number, "1"));
    CHECK (pt.match(TokenType::Number, "2"));
    CHECK (pt.match(TokenType::Number, "3"));
    CHECK (pt.match(TokenType::Number, "4"));
    CHECK (pt.match(TokenType::Number, "5"));
    CHECK ( ! pt.match(TokenType::EndOfFile, ""));
}

TEST (parser_test_group, test_token_location)
{
    Lexer l("1");
    auto tokens = l.tokenize();
    Parser p(tokens);
    ParserTester pt(p);

    CHECK_EQUAL (std::string{"line 1 col 1"}, pt.tokenLocation());
    pt.advance();
    CHECK_EQUAL (std::string{"<EOF>"}, pt.tokenLocation());
}
