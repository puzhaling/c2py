#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "ast.h"


struct SymbolTableTester;

/*
 SymbolTable: стек областей видимости.
 Сохраняет соответствие имени -> ASTNode* (объявление переменной/функции).
*/
class SymbolTable {
public:
    SymbolTable();

    using ScopeContainer = std::vector<
        std::unordered_map<std::string, ASTNode*>
    >;
    
    void pushScope();
    void popScope();

    // объявление символа в текущей области
    void declare(const std::string& name, ASTNode* decl);

    // поиск символа начиная с текущей области и вверх
    ASTNode* lookup(const std::string& name) const;

    bool isDeclaredInCurrentScope(const std::string& name) const;

    friend struct SymbolTableTester;
protected:
    const ScopeContainer& getScopes() const;

private:
    std::vector<std::unordered_map<std::string, ASTNode*>> scopes;
};


struct SymbolTableTester
{
    SymbolTable& st;

    explicit SymbolTableTester (SymbolTable &st)
        :   st (st)
    {
    }

    const SymbolTable::ScopeContainer& getScopes() const;
};
