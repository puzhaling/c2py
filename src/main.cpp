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
#include "semantic.h"
#include "symbol_table.h"
#include "code_generator.h"
#include "gui.h"

#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>


Fl_Text_Buffer *inputBuf = new Fl_Text_Buffer();
Fl_Text_Buffer *outputBuf = new Fl_Text_Buffer();

void ui_import(Fl_Button*, void*) {
    Fl_Native_File_Chooser fc;
    fc.title("c2py / Import C file");
    fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fc.filter("C File\t*.{c,cpp,h}");
    if (fc.show() == 0) {
        inputBuf->loadfile(fc.filename());
    }
}

void ui_export(Fl_Button*, void*) {
    Fl_Native_File_Chooser fc;
    fc.title("c2py / Import Python file");
    fc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fc.filter("Python File\t*.py");
    fc.preset_file("translated.py");
    if (fc.show() == 0) {
        outputBuf->savefile(fc.filename());
    }
}

void ui_translate(Fl_Button*, void*) {
    std::string code = inputBuf->text();
    if (code.empty()) return;
    try {
        //std::string code = loadSource(argc, argv);
        
        // Определяем источник кода
        //std::string sourceInfo = (argc > 1) ? std::string("File: ") + argv[1] : "Built-in example";

        // 1) Лексический анализ
        Lexer lexer(code);
        auto tokens = lexer.tokenize();

        // 2) Синтаксический анализ → AST
        Parser parser(tokens);
        auto program = parser.parseProgram();

        // 3) Семантический анализ
        SymbolTable symbolTable;
        SemanticAnalyzer semanticAnalyzer(symbolTable);
        if (!semanticAnalyzer.analyze(program)) {
            std::string error = "=== Semantic Analysis Errors ===\n";
            for (const auto& err : semanticAnalyzer.getErrors()) {
                error += err + "\n";
            }
            outputBuf->text(error.c_str());
            return;
        }
        
        // Вывод предупреждений если есть
        if (!semanticAnalyzer.getWarnings().empty()) {
            std::string warning = "# === Warnings ===\n";
            for (const auto& warn : semanticAnalyzer.getWarnings()) {
                warning += "# " + warn + "\n";
            }
            outputBuf->text(warning.c_str());
        }

        // 4) Генерация Python кода
        CodeGenerator codeGen(&semanticAnalyzer);
        std::string pythonCode = codeGen.generate(program.get());
        
        // 5) Вывод
        outputBuf->text(pythonCode.c_str());

    } catch (const std::exception& ex) {
        outputBuf->text(ex.what());
    }
}

int main(int argc, char** argv) {
    UserInterface* ui = new UserInterface();
    Fl_Window* window = ui->make_window();
    ui->input_window->buffer(inputBuf);
    ui->input_window->linenumber_width(30); 
    ui->input_window->linenumber_size(12);
    ui->output_window->buffer(outputBuf);
    inputBuf->text(R"(int main() {
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
    })");

    window->show(argc, argv);
    return Fl::run();
}
