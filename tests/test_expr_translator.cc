#include	"expr_translator.h"
#include	"token.h"

#include	<CppUTest/TestHarness.h>
#include	<vector>
#include	<string>


static ExpressionTranslator e;
static std::vector<Token> toks = {
	Token{ TokenType::Unknown, "", 0, 0 }
};

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
	toks[0] = Token{ TokenType::Operator, "&&", 0, 0 };
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
	toks[0] = Token{ TokenType::Operator, "++", 0, 0 };
	STRCMP_EQUAL ("+= 1", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "--";
	STRCMP_EQUAL ("-= 1", strip (e.translate(toks)).c_str());
}

TEST (expr_translator_test_group, test_translate_left_lexeme_untouched)
{
	toks[0] = Token{ TokenType::Unknown, "DONT_CHANGE_ME", 0, 0 };
	STRCMP_EQUAL ("DONT_CHANGE_ME", strip(e.translate(toks)).c_str());

	toks[0].lexeme = "";
	STRCMP_EQUAL ("", strip(e.translate(toks)).c_str());
}

TEST (expr_translator_test_group, test_translate_separates_tokens_with_single_space)
{
	toks[0] = Token{ TokenType::Unknown, "DONT_CHANGE_ME", 0, 0 };
	toks.push_back(toks[0]);
	toks.push_back(toks[0]);

	STRCMP_EQUAL ("DONT_CHANGE_ME DONT_CHANGE_ME DONT_CHANGE_ME",
				strip(e.translate(toks)).c_str());
}
