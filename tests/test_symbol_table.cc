#include    "symbol_table.h"

#include    <CppUTest/TestHarness.h>


TEST_GROUP (symbol_table_test_group)
{
};

TEST (symbol_table_test_group, test_init_global_scope)
{
    SymbolTable st;
    SymbolTableTester stt(st);

    CHECK (stt.getScopes().size() == 1);
}

TEST (symbol_table_test_group, test_push_scope)
{
    SymbolTable st;
    SymbolTableTester stt(st);
    const auto& scopes = stt.getScopes();

    CHECK (scopes.size() == 1);
    st.pushScope();
    CHECK (scopes.size() == 2);
    st.pushScope();
    CHECK (scopes.size() == 3);
}

TEST (symbol_table_test_group, test_pop_scope)
{
    SymbolTable st;
    SymbolTableTester stt(st);
    const auto& scopes = stt.getScopes();

    st.pushScope();
    st.pushScope();
    CHECK (scopes.size() == 3);
    st.popScope();
    CHECK (scopes.size() == 2);
    st.popScope();
    CHECK (scopes.size() == 1);
    st.popScope();
    CHECK (scopes.size() == 0);
    st.popScope();
    CHECK (scopes.size() == 0);
    st.popScope();
    CHECK (scopes.size() == 0);
}

TEST (symbol_table_test_group, test_declare_with_no_scopes_creates_global_scope)
{
    SymbolTable st;
    SymbolTableTester stt(st);
    const auto& scopes = stt.getScopes();
    ASTNode astnode;

    st.popScope();

    CHECK (scopes.size() == 0);
    st.declare("XXX", &astnode);
    CHECK (scopes.size() == 1);
}

TEST (symbol_table_test_group, test_declare_places_new_identifier_in_outermost_scope)
{
    SymbolTable st;
    SymbolTableTester stt(st);
    const auto& scopes = stt.getScopes();
    ASTNode astnode;

    st.pushScope();
    st.pushScope();
    st.pushScope();
    st.pushScope();

    st.declare("XXX", &astnode);

    // Make sure that the newly declared identifier is not
    // present in any scope other than the outermost one
    CHECK (scopes[0].find("XXX") == scopes[0].end());
    CHECK (scopes[1].find("XXX") == scopes[1].end());
    CHECK (scopes[2].find("XXX") == scopes[2].end());
    CHECK (scopes[3].find("XXX") == scopes[3].end());

    CHECK (scopes[4].find("XXX") != scopes[4].end());
}

TEST (symbol_table_test_group, test_declare_overwrites_identifier_with_same_name)
{
    SymbolTable st;
    SymbolTableTester stt(st);
    const auto& scopes = stt.getScopes();
    ASTNode node_A, node_B;

    st.declare("XXX", &node_A);

    auto iter = scopes[0].find("XXX");

    st.declare("XXX", &node_B);

    STRCMP_EQUAL ( (iter->first).c_str(), "XXX");
    CHECK (iter->second == &node_B);
}

TEST (symbol_table_test_group,
    test_isDeclaredInCurrentScope_stop_trying_when_there_are_no_scopes)
{
    SymbolTable st;
    SymbolTableTester stt(st);

    st.popScope();

    CHECK ( ! st.isDeclaredInCurrentScope("XXX"));
}

TEST (symbol_table_test_group, test_isDeclaredInCurrentScope)
{
    SymbolTable st;
    SymbolTableTester stt(st);

    st.pushScope();

    st.declare("REALLY_COOL_IDENTIFIER", nullptr);
    CHECK (st.isDeclaredInCurrentScope("REALLY_COOL_IDENTIFIER"));

    st.popScope();
    CHECK ( ! st.isDeclaredInCurrentScope("REALLY_COOL_IDENTIFIER"));

    st.popScope();
    CHECK ( ! st.isDeclaredInCurrentScope("REALLY_COOL_IDENTIFIER"));
}

TEST (symbol_table_test_group, test_lookup)
{
    SymbolTable st;
    ASTNode astnode;

    CHECK (st.lookup("NON_EXISTING_IDENTIFIER") == nullptr);
    st.declare("XXX", &astnode);
    CHECK (st.lookup("XXX") == &astnode);
    st.popScope();
    CHECK (st.lookup("XXX") == nullptr);
}

IGNORE_TEST (symbol_table_test_group, test_lookup_identifier_shadowing)
{
    SymbolTable st;
    SymbolTableTester stt(st);
    ASTNode node_A, node_B;

    st.declare("THE_SAME_IDENTIFIER", &node_A);
    st.pushScope();
    st.declare("THE_SAME_IDENTIFIER", &node_B);

    CHECK (st.lookup("THE_SAME_IDENTIFIER") == &node_B);
    st.popScope();
    CHECK (st.lookup("THE_SAME_IDENTIFIER") == &node_A);
}
