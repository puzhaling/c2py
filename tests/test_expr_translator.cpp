#include	"expr_translator.h"
#include	"token.h"

#include	<CppUTest/TestHarness.h>
#include	<vector>
#include	<string>


static
std::string strip (const std::string &instring)
{
	const char *whitespace = " \t\n\r";

	const std::size_t first = instring.find_first_not_of (whitespace);
	if (first == std::string::npos)
		return "";

	const std::size_t last = instring.find_last_not_of (whitespace);
	const std::size_t range = last - first + 1;

	return instring.substr (first, range);
}


TEST_GROUP (expr_translator_test_group)
{
};

TEST (expr_translator_test_group, test_map_operators)
{
	ExpressionTranslator e;
	std::vector<Token> toks = {
		{ TokenType::Operator, "&&", 0, 0 }
	};

	STRCMP_EQUAL ("and", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "||";
	STRCMP_EQUAL ("or", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "!";
	STRCMP_EQUAL ("not", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "TRASH";
	STRCMP_EQUAL ("TRASH", strip(e.translate(toks)).c_str());
}

TEST (expr_translator_test_group, test_translate_inc_dec)
{
	ExpressionTranslator e;
	std::vector<Token> toks = {
		{ TokenType::Operator, "++", 0, 0 }
	};

	STRCMP_EQUAL ("+= 1", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "--";
	STRCMP_EQUAL ("-= 1", strip (e.translate(toks)).c_str());
}

TEST (expr_translator_test_group, test_translate_left_lexeme_untouched)
{
	ExpressionTranslator e;
	std::vector<Token> toks = {
		{ TokenType::Unknown, "DONT_CHANGE_ME", 0, 0 }
	};

	STRCMP_EQUAL ("DONT_CHANGE_ME", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "";
	STRCMP_EQUAL ("", strip(e.translate(toks)).c_str());
}

TEST (expr_translator_test_group, test_translate_separates_tokens_with_single_space)
{
	ExpressionTranslator e;
	std::vector<Token> toks = {
		{ TokenType::Unknown, "DONT_CHANGE_ME", 0, 0 },
		{ TokenType::Unknown, "DONT_CHANGE_ME", 0, 0 },
		{ TokenType::Unknown, "DONT_CHANGE_ME", 0, 0 }
	};

	STRCMP_EQUAL ("DONT_CHANGE_ME DONT_CHANGE_ME DONT_CHANGE_ME",
				strip(e.translate(toks)).c_str());
}
