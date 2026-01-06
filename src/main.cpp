/**
 * \mainpage C to Python translator
 *
 * This project implements a translator that converts programs
 * written in a subset of the C programming language into semantically
 * equivalent Python code. The translator performs lexical, syntactic,
 * and semantic analysis of the input C code, followed by the generation
 * of valid Python source code that replicates the original program's logic
 * and behavior.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

#include "lexer.h"
#include "parser.h"
#include "ast.h"

// Загружает исходник либо из файла argv[1], либо возвращает встроенный пример.
static std::string loadSource(int argc, char** argv) {
    if (argc > 1) {
        std::ifstream in(argv[1]);
        if (!in) {
            std::ostringstream err;
            err << "Cannot open input file: " << argv[1];
            throw std::runtime_error(err.str());
        }
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    // Встроенный пример
 return R"(
int main() {
    int i = 0;
    int j = 0;
    int k = 10;

    for (i = 0; i < 5; i++) { j++; k--; }

    while (i < 10) {
        if (i == 7) break;
        if (i % 2 == 0) { i++; continue; }
        j += i;
        i++;
    }

    do {
        k += 2;
    } while (k < 20);

    if (i > j) { i -= 1; }
    else if (i == j) { j += 1; }
    else { k = 0; }

    ++i;
    --j;
    i = -i;

    i += 2;
    j -= 3;
    k *= 2;
    k %= 5;

    return 42;
}
)";
}

int main(int argc, char** argv) {
    try {
        std::string code = loadSource(argc, argv);

        // 1) Лексический анализ
        Lexer lexer(code);
        auto tokens = lexer.tokenize();

        // 2) Синтаксический анализ → AST
        Parser parser(tokens);
        auto program = parser.parseProgram();

        // 3) Печать AST (удобно для отладки)
        printProgramAST(program.get(), std::cout);

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
