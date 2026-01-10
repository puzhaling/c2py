#include    "symbol_table.h"


SymbolTable::SymbolTable()
{
    pushScope();
}

void
SymbolTable::pushScope()
{
    scopes.emplace_back();
}

void
SymbolTable::popScope()
{
    if (!scopes.empty()) scopes.pop_back();
}

// объявление символа в текущей области
void
SymbolTable::declare(const std::string& name, ASTNode* decl) {
    if (scopes.empty()) pushScope();
    scopes.back()[name] = decl;
}

// поиск символа начиная с текущей области и вверх
ASTNode*
SymbolTable::lookup(const std::string& name) const
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    return nullptr;
}

bool
SymbolTable::isDeclaredInCurrentScope(const std::string& name) const
{
    if (scopes.empty()) return false;
    return scopes.back().count(name) > 0;
}

const SymbolTable::ScopeContainer&
SymbolTable::getScopes() const
{
    return scopes;
}

const SymbolTable::ScopeContainer&
SymbolTableTester::getScopes() const
{
    return st.getScopes();
}
