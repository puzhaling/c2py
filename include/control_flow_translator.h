#pragma once
#include <vector>
#include <string>
#include "token.h"

class ControlFlowTranslator {
public:
    std::string translate(const std::vector<Token>& tokens);

private:
    int indent = 0;
    std::string indentStr() const;

    // Вспомогательные методы
    size_t findMatching(const std::vector<Token>& toks, size_t start, const std::string& openSym, const std::string& closeSym) const;
    std::vector<Token> sliceTokens(const std::vector<Token>& toks, size_t a, size_t b) const; // [a,b)
};
