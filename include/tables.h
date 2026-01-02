/**
 * \file
 * \brief C language specific words
 *
 * Here are collected C's keywords, types, operators and separators.
*/


#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>


static const std::unordered_set<std::string> KEYWORDS = {
    "if", "else", "for", "while", "do",
    "break", "continue", "return",
    "switch", "case", "default"
};

static const std::unordered_set<std::string> TYPES = {
    "int", "float", "double", "char", "bool", "void"
};

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

static const std::unordered_set<char> SEPARATORS = {
    '(', ')', '{', '}', '[', ']',
    ';', ',', '.'
};
