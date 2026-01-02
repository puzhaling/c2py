/**
 * \mainpage C to Python translator
 *
 * This project implements a translator that converts programs
 * written in a subset of the C programming language into semantically
 * equivalent Python code. The translator performs lexical, syntactic,
 * and semantic analysis of the input C code, followed by the generation
 * of valid Python source code that replicates the original program's logic
 * and behavior.
 *
 * \authors
 * - [Maxim Gorlatenko](https://github.com/mqwl)
 * - [Danila Kabanov](https://github.com/Danila121)
 * - [German Puzhalin](https://github.com/puzhaling)
 * - [Danila Porohov](https://github.com/blackmagicqqqqdwqd)
*/


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
