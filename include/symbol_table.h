#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "ast.h"

/*
 SymbolTable: стек областей видимости.
 Сохраняет соответствие имени -> ASTNode* (объявление переменной/функции).
*/
class SymbolTable {
public:
    SymbolTable() { pushScope(); }

    void pushScope() {
        scopes.emplace_back();
    }

    void popScope() {
        if (!scopes.empty()) scopes.pop_back();
    }

    // объявление символа в текущей области
    void declare(const std::string& name, ASTNode* decl) {
        if (scopes.empty()) pushScope();
        scopes.back()[name] = decl;
    }

    // поиск символа начиная с текущей области и вверх
    ASTNode* lookup(const std::string& name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) return found->second;
        }
        return nullptr;
    }

    bool isDeclaredInCurrentScope(const std::string& name) const {
        if (scopes.empty()) return false;
        return scopes.back().count(name) > 0;
    }

private:
    std::vector<std::unordered_map<std::string, ASTNode*>> scopes;
};
