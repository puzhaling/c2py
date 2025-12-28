// Таблица ключевых слов

#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>

// Ключевые слова
static const std::unordered_set<std::string> KEYWORDS = {
    "if", "else", "for", "while", "do",
    "break", "continue", "return",
    "switch", "case", "default"
};
// Зарезервированные имена
static const std::unordered_set<std::string> TYPES = {
    "int", "float", "double", "char", "bool", "void"
};
// Операторы C
static const std::unordered_set<std::string> OPERATORS = {
    "+", "-", "*", "/", "%",
    "++", "--",
    "==", "!=", "<", ">", "<=", ">=",
    "&&", "||", "!",
    "&", "|", "^", "~", "<<", ">>",
    "=", "+=", "-=", "*=", "/=", "%=",
    "&=", "|=", "^=", "<<=", ">>=",
    "?", ":"
};
// Разделители
static const std::unordered_set<char> SEPARATORS = {
    '(', ')', '{', '}', '[', ']',
    ';', ',', '.'
};
