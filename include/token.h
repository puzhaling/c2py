// Код C разбивается на токены,
// это header токена

#pragma once
#include <string>

// Типы токенов
enum class TokenType {
    Keyword,
    Identifier,
    Number,
    String,
    Operator,
    Separator,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type; // тип токена
    std::string lexeme; // содержимое токена
    int line; // строка кода
    int column; // столбец кода
    Token(TokenType t, const std::string& l, int ln, int col)
        : type(t), lexeme(l), line(ln), column(col) {}
};
