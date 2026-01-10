#include	"CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP (expr_translator_test_group);
IMPORT_TEST_GROUP (lexer_test_group);
IMPORT_TEST_GROUP (parser_test_group);

int main (int ac, char **av)
{
	return CommandLineTestRunner::RunAllTests (ac, av);
}
