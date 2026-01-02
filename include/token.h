#pragma once
#include <string>

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

/**
 * \brief Associates a portion of the source text
 * with the necessary metainformation for further processing.
*/
struct Token {
    TokenType type;     /**< A portion's type */
    std::string lexeme; /**< A portion of the source text */
    int line;           /**< The line from which the source text portion is taken */
    int column;         /**< The column from which the source text portion is taken */
    Token(TokenType t, const std::string& l, int ln, int col)
        : type(t), lexeme(l), line(ln), column(col) {}
};
