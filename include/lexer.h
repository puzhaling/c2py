// Header файл лексического анализатора

#pragma once
#include "token.h"
#include "tables.h"
#include <vector>

class Lexer {
public:
    // Конструктор, принимающий на вход код (на языке C)
    explicit Lexer(const std::string& src);
    // Основная функция лексического анализа
    std::vector<Token> tokenize();
private:
    char peek() const; // Просмотр текущего символа
    char get(); // Чтение текущего символа
    bool eof() const; // Проверка, закончился ли код
    void skipWhitespace(); // Пропуск пробелов, переносов строк и табуляции
    Token readIdentifierOrKeyword(); // Чтение идентификатора/ключевого слова
    Token readNumber(); // Чтение числа
    Token readOperator(); // Чтение оператора
    Token readSeparator(); // Чтение разделителя
    const std::string source; // Код, поступающий на вход
    size_t pos; // Позиция читаемого символа
    int line; // Номер строки
    int column; // Номер столбца
};
