// g++ -o test main.cpp lexer.cpp expr_translator.cpp
// ./test

#include "lexer.h"
#include "expr_translator.h"
#include <iostream>

int main() {
    std::string code = "a && b || !c; x++;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    ExpressionTranslator tr;
    std::cout << tr.translate(tokens) << std::endl;
}
