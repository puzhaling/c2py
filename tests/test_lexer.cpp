#include    "lexer.h"
#include    <iostream>
#include	<CppUTest/TestHarness.h>


static auto
get_tokens (const char *instring)
{
	Lexer l (instring);
	auto tokens = l.tokenize ();

	// Get rid of EOF token.
	tokens.pop_back ();

	return tokens;
}


TEST_GROUP (lexer_test_group)
{
};


TEST (lexer_test_group, test_tokenize_numbers)
{
	auto tkns = get_tokens ("1 2.5 -1 -2.5");
    
    CHECK (tkns[0].type == TokenType::Number);
    STRCMP_EQUAL ("1", tkns[0].lexeme.c_str ());

    CHECK (tkns[1].type == TokenType::Number);
    STRCMP_EQUAL ("2.5", tkns[1].lexeme.c_str ());

	CHECK (tkns[2].type == TokenType::Number);
    STRCMP_EQUAL ("-1", tkns[2].lexeme.c_str ());

	CHECK (tkns[3].type == TokenType::Number);
    STRCMP_EQUAL ("-2.5", tkns[3].lexeme.c_str ());
}


TEST (lexer_test_group, test_tokenize_identifiers)
{
	auto tkns = get_tokens ("abc _abc");

    CHECK (tkns[0].type == TokenType::Identifier);
    STRCMP_EQUAL ("abc", tkns[0].lexeme.c_str ());

    CHECK (tkns[1].type == TokenType::Identifier);
    STRCMP_EQUAL ("_abc", tkns[1].lexeme.c_str ());
}


TEST (lexer_test_group, test_tokenize_keywords)
{
	auto tkns = get_tokens (
		"if else for while do"
        " break continue return"
        " switch case default"
	);

    for (const auto &tkn : tkns) {
        CHECK (TokenType::Keyword == tkn.type);
	}

    STRCMP_EQUAL ("if",       tkns[0].lexeme.c_str ());
    STRCMP_EQUAL ("else",     tkns[1].lexeme.c_str ());
    STRCMP_EQUAL ("for",      tkns[2].lexeme.c_str ());
    STRCMP_EQUAL ("while",    tkns[3].lexeme.c_str ());
    STRCMP_EQUAL ("do",       tkns[4].lexeme.c_str ());
    STRCMP_EQUAL ("break",    tkns[5].lexeme.c_str ());
    STRCMP_EQUAL ("continue", tkns[6].lexeme.c_str ());
    STRCMP_EQUAL ("return",   tkns[7].lexeme.c_str ());
    STRCMP_EQUAL ("switch",   tkns[8].lexeme.c_str ());
    STRCMP_EQUAL ("case",     tkns[9].lexeme.c_str ());
    STRCMP_EQUAL ("default",  tkns[10].lexeme.c_str ());
}


TEST (lexer_test_group, test_tokenize_types)
{
	auto tkns = get_tokens (
		"int float double char bool void"
	);

    for (const auto &tkn : tkns) {
        CHECK (TokenType::Keyword == tkn.type);
	}

    STRCMP_EQUAL ("int",    tkns[0].lexeme.c_str ());
    STRCMP_EQUAL ("float",  tkns[1].lexeme.c_str ());
    STRCMP_EQUAL ("double", tkns[2].lexeme.c_str ());
    STRCMP_EQUAL ("char",   tkns[3].lexeme.c_str ());
    STRCMP_EQUAL ("bool",   tkns[4].lexeme.c_str ());
    STRCMP_EQUAL ("void",   tkns[5].lexeme.c_str ());
}


TEST (lexer_test_group, test_tokenize_operators)
{
	auto tkns = get_tokens (
		"+ - * / %"
		" ++ --"
		" == != < > <= >="
		" && || !"
		" & | ^ ~ << >>"
		" = += -= *= /= %="
		" &= |= ^= <<= >>="
		" ? :"
	);

    for (const auto &tkn : tkns) {
        CHECK (TokenType::Operator == tkn.type);
	}

    STRCMP_EQUAL ("+",   tkns[0].lexeme.c_str ());
    STRCMP_EQUAL ("-",   tkns[1].lexeme.c_str ());
    STRCMP_EQUAL ("*",   tkns[2].lexeme.c_str ());
    STRCMP_EQUAL ("/",   tkns[3].lexeme.c_str ());
    STRCMP_EQUAL ("%",   tkns[4].lexeme.c_str ());

    STRCMP_EQUAL ("++",  tkns[5].lexeme.c_str ());
	STRCMP_EQUAL ("--",  tkns[6].lexeme.c_str ());

	STRCMP_EQUAL ("==",  tkns[7].lexeme.c_str ());
	STRCMP_EQUAL ("!=",  tkns[8].lexeme.c_str ());
	STRCMP_EQUAL ("<",   tkns[9].lexeme.c_str ());
	STRCMP_EQUAL (">",   tkns[10].lexeme.c_str ());
	STRCMP_EQUAL ("<=",  tkns[11].lexeme.c_str ());
	STRCMP_EQUAL (">=",  tkns[12].lexeme.c_str ());

	STRCMP_EQUAL ("&&",  tkns[13].lexeme.c_str ());
	STRCMP_EQUAL ("||",  tkns[14].lexeme.c_str ());
	STRCMP_EQUAL ("!",   tkns[15].lexeme.c_str ());

	STRCMP_EQUAL ("&",   tkns[16].lexeme.c_str ());
	STRCMP_EQUAL ("|",   tkns[17].lexeme.c_str ());
	STRCMP_EQUAL ("^",   tkns[18].lexeme.c_str ());
	STRCMP_EQUAL ("~",   tkns[19].lexeme.c_str ());
	STRCMP_EQUAL ("<<",  tkns[20].lexeme.c_str ());
	STRCMP_EQUAL (">>",  tkns[21].lexeme.c_str ());

	STRCMP_EQUAL ("=",   tkns[22].lexeme.c_str ());
	STRCMP_EQUAL ("+=",  tkns[23].lexeme.c_str ());
	STRCMP_EQUAL ("-=",  tkns[24].lexeme.c_str ());
	STRCMP_EQUAL ("*=",  tkns[25].lexeme.c_str ());
	STRCMP_EQUAL ("/=",  tkns[26].lexeme.c_str ());
	STRCMP_EQUAL ("%=",  tkns[27].lexeme.c_str ());

	STRCMP_EQUAL ("&=",  tkns[28].lexeme.c_str ());
	STRCMP_EQUAL ("|=",  tkns[29].lexeme.c_str ());
	STRCMP_EQUAL ("^=",  tkns[30].lexeme.c_str ());
	STRCMP_EQUAL ("<<=", tkns[31].lexeme.c_str ());
	STRCMP_EQUAL (">>=", tkns[32].lexeme.c_str ());

	STRCMP_EQUAL ("?",   tkns[33].lexeme.c_str ());
	STRCMP_EQUAL (":",   tkns[34].lexeme.c_str ());
}

TEST (lexer_test_group, test_tokenize_separators)
{
	auto tkns = get_tokens (
		"( ) { } [ ]"
		"; , ."
	);

    for (const auto &tkn : tkns) {
        CHECK (TokenType::Separator == tkn.type);
	}

	STRCMP_EQUAL ("(", tkns[0].lexeme.c_str ());
	STRCMP_EQUAL (")", tkns[1].lexeme.c_str ());
	STRCMP_EQUAL ("{", tkns[2].lexeme.c_str ());
	STRCMP_EQUAL ("}", tkns[3].lexeme.c_str ());
	STRCMP_EQUAL ("[", tkns[4].lexeme.c_str ());
	STRCMP_EQUAL ("]", tkns[5].lexeme.c_str ());
	STRCMP_EQUAL (";", tkns[6].lexeme.c_str ());
	STRCMP_EQUAL (",", tkns[7].lexeme.c_str ());
	STRCMP_EQUAL (".", tkns[8].lexeme.c_str ());
}

TEST (lexer_test_group, test_skip_whitespace)
{
	Lexer l (" \f\n\r\t\v");
	auto tkns = l.tokenize ();

	CHECK (tkns.size () == 1 && tkns[0].type == TokenType::EndOfFile);
}