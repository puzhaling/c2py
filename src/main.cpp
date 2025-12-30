//c++ -Iinclude src/lexer.cpp src/expr_translator.cpp src/control_flow_translator.cpp src/main.cpp -o translator
//./translator


#include "lexer.h"
#include "expr_translator.h"
#include "control_flow_translator.h"
#include <iostream>

int main() {
    std::string code =
        "int main() { "
        "int i = 0; "
        "for (i = 0; i < 3; i++) { x++; } "
        "if (a && b || !c) { x++; } else { x -= 1; } "
        "do { y++; } while (y < 5); "
        "return 0; "
        "}";

    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    ControlFlowTranslator tr;
    std::cout << tr.translate(tokens) << std::endl;

    return 0;
}
