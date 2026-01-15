#include    "lexer.h"
#include    "parser.h"
#include    "parser_tester.h"

#include    <iostream>
#include    <CppUTest/TestHarness.h>



static bool
tokens_eq (const Token &fst, const Token &snd)
{
    return fst.type == snd.type && fst.lexeme == snd.lexeme;
}


TEST_GROUP (parser_test_group)
{
    std::unique_ptr<Parser> parser;
    std::unique_ptr<ParserTester> pt;
    
    void
    setup () override
    {

    }
    
    void
    teardown () override
    {
        pt.reset ();
        parser.reset ();
    }
    
    void
    createParser (const std::string& code)
    {
        Lexer l (code);
        auto tokens = l.tokenize ();
        parser = std::make_unique<Parser> (tokens);
        pt = std::make_unique<ParserTester> (*parser);
    }
};


TEST (parser_test_group, test_at_end)
{
    createParser ("");

    CHECK (pt->atEnd() == true);
}

TEST (parser_test_group, test_peek)
{
    createParser ("1 2 3");

    CHECK ( tokens_eq(pt->peek(),pt->getTokens()[0]) );
}

TEST (parser_test_group, test_advance)
{
    createParser ("1 2 3 4 5");
    const auto& tokens = pt->getTokens ();

    CHECK ( tokens_eq(pt->advance(),tokens[0]) );
    CHECK ( tokens_eq(pt->advance(),tokens[1]) );
    CHECK ( tokens_eq(pt->advance(),tokens[2]) );
    CHECK ( tokens_eq(pt->advance(),tokens[3]) );
    CHECK ( tokens_eq(pt->advance(),tokens[4]) );
    CHECK ( tokens_eq(pt->advance(),tokens[4]) );
    CHECK ( tokens_eq(pt->advance(),tokens[4]) );
    CHECK ( tokens_eq(pt->advance(),tokens[4]) );
    CHECK ( tokens_eq(pt->advance(),tokens[4]) );
}

TEST (parser_test_group, test_previous)
{
    createParser ("1 2 3 4 5");
    const auto& tokens = pt->getTokens ();

    CHECK ( tokens_eq(pt->previous(),tokens[0]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[0]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[1]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[2]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[3]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[4]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[4]) );
    pt->advance ();
    CHECK ( tokens_eq(pt->previous(),tokens[4]) );
    pt->advance ();
}

TEST (parser_test_group, test_check)
{
    createParser ("1 2 3 4 5");
    auto tokens = pt->getTokens ();

    // Be aware that Parser::check returns FALSE in this case.  See the source
    CHECK ( ! pt->check(TokenType::Unknown, ""));

    CHECK (pt->check(TokenType::Number, "1"));

    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();
    pt->advance ();

    CHECK ( ! pt->check(TokenType::EndOfFile, ""));
}

TEST (parser_test_group, test_match)
{
    createParser ("1 2 3 4 5");

    CHECK (pt->match(TokenType::Number, "1"));
    CHECK (pt->match(TokenType::Number, "2"));
    CHECK (pt->match(TokenType::Number, "3"));
    CHECK (pt->match(TokenType::Number, "4"));
    CHECK (pt->match(TokenType::Number, "5"));
    CHECK ( ! pt->match(TokenType::EndOfFile, ""));
}

TEST (parser_test_group, test_token_location)
{
    createParser ("1");

    CHECK_EQUAL (std::string{"line 1 col 1"}, pt->tokenLocation());
    pt->advance ();
    CHECK_EQUAL (std::string{"<EOF>"}, pt->tokenLocation());
}


TEST (parser_test_group, test_parsePrimary_number)
{
    createParser ("42");
    
    auto expr = pt->parsePrimary ();
    CHECK_TRUE (expr != nullptr);
    CHECK_TRUE (dynamic_cast<NumberExpr*>(expr.get()) != nullptr);
    CHECK_EQUAL ("42", dynamic_cast<NumberExpr*>(expr.get())->value);
}

TEST (parser_test_group, test_parsePrimary_identifier)
{
    createParser ("x");
    
    auto expr = pt->parsePrimary ();
    CHECK_TRUE (expr != nullptr);
    auto id = dynamic_cast<IdentifierExpr*> (expr.get());
    CHECK_TRUE (id != nullptr);
    CHECK_EQUAL ("x", id->name);
}

TEST (parser_test_group, test_parsePrimary_error)
{
    createParser ("+");
    
    CHECK_THROWS (std::runtime_error, pt->parsePrimary());
}

TEST (parser_test_group, test_parse_unary_prefix)
{
    createParser ("-x");
    
    auto expr = pt->parseUnary ();
    CHECK_TRUE (expr != nullptr);
    auto unary = dynamic_cast<UnaryExpr*> (expr.get());
    CHECK_TRUE (unary != nullptr);
    CHECK_EQUAL ("-", unary->op);
}

TEST (parser_test_group, test_parse_unary_not)
{
    createParser ("!true");
    
    auto expr = pt->parseUnary ();
    CHECK_TRUE (expr != nullptr);
    auto unary = dynamic_cast<UnaryExpr*> (expr.get());
    CHECK_TRUE (unary != nullptr);
    CHECK_EQUAL ("!", unary->op);
}


TEST (parser_test_group, test_parse_unary_pre_increment)
{
    createParser ("++x");
    
    auto expr = pt->parseUnary ();
    CHECK_TRUE (expr != nullptr);
    auto unary = dynamic_cast<UnaryExpr*> (expr.get());
    CHECK_TRUE (unary != nullptr);
    CHECK_EQUAL ("++", unary->op);
}

TEST (parser_test_group, test_parse_postfix_trivial)
{
    createParser ("x");
    
    auto expr = pt->parsePostfix ();
    CHECK_TRUE (expr != nullptr);
    CHECK_TRUE (dynamic_cast<IdentifierExpr*>(expr.get()) != nullptr);
}

TEST (parser_test_group, test_parse_postfix_increment)
{
    createParser ("x++");
    
    auto expr = pt->parsePostfix ();
    CHECK_TRUE (expr != nullptr);
    auto unary = dynamic_cast<UnaryExpr*> (expr.get());
    CHECK_TRUE (unary != nullptr);
    CHECK_EQUAL ("++", unary->op);
}

TEST (parser_test_group, test_parse_postfix_decrement)
{
    createParser ("y--");
    
    auto expr = pt->parsePostfix ();
    CHECK_TRUE (expr != nullptr);
    auto unary = dynamic_cast<UnaryExpr*> (expr.get());
    CHECK_TRUE (unary != nullptr);
    CHECK_EQUAL ("--", unary->op);
}

TEST (parser_test_group, test_parse_multiplicative_multiply)
{
    createParser ("a * b");
    
    auto expr = pt->parseMultiplicative ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("*", binExpr->op);
}

TEST (parser_test_group, test_parse_multiplicative_divide)
{
    createParser ("x / y");
    
    auto expr = pt->parseMultiplicative ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("/", binExpr->op);
}

TEST (parser_test_group, test_parse_multiplicative_modulo)
{
    createParser ("a % b");
    
    auto expr = pt->parseMultiplicative ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("%", binExpr->op);
}

TEST (parser_test_group, test_parse_additive_plus)
{
    createParser ("a + b");
    
    auto expr = pt->parseAdditive ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("+", binExpr->op);
}

TEST (parser_test_group, test_parseAdditive_minus)
{
    createParser ("x - y");
    
    auto expr = pt->parseAdditive ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("-", binExpr->op);
}


TEST (parser_test_group, test_parse_additive_chained)
{
    createParser ("a + b - c");
    
    // Must be parsed as ((a + b) - c)
    auto expr = pt->parseAdditive ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("-", binExpr->op);
}

TEST (parser_test_group, test_parse_relational_less)
{
    createParser ("a < b");
    
    auto expr = pt->parseRelational ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("<", binExpr->op);
}

TEST (parser_test_group, test_parse_relational_greater_equal)
{
    createParser ("x >= y");
    
    auto expr = pt->parseRelational ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL (">=", binExpr->op);
}

TEST (parser_test_group, test_parse_equality_equal)
{
    createParser ("a == b");
    
    auto expr = pt->parseEquality ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("==", binExpr->op);
}

TEST (parser_test_group, test_parse_equality_not_equal)
{
    createParser ("x != y");
    
    auto expr = pt->parseEquality ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("!=", binExpr->op);
}

TEST (parser_test_group, test_parse_logical_and_trivial)
{
    createParser ("a && b");
    
    auto expr = pt->parseLogicalAnd ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("&&", binExpr->op);
}

TEST (parser_test_group, test_parse_logical_or_trivial)
{
    createParser ("a || b");
    
    auto expr = pt->parseLogicalOr ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("||", binExpr->op);
}

TEST (parser_test_group, test_parse_assignment_simple)
{
    createParser ("x = 5");
    
    auto expr = pt->parseAssignment ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("=", binExpr->op);
}

TEST (parser_test_group, test_parse_assignment_compound)
{
    createParser ("x += 10");
    
    auto expr = pt->parseAssignment ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("+=", binExpr->op);
}

TEST (parser_test_group, test_parse_expression_trivial)
{
    createParser ("x + y");
    
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("+", binExpr->op);
}

TEST (parser_test_group, test_parse_var_decl_statement_no_init)
{
    createParser ("int x;");
    
    auto stmt = pt->parseVarDeclStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto varDecl = dynamic_cast<VarDecl*> (stmt.get());
    CHECK_TRUE (varDecl != nullptr);
    CHECK_EQUAL ("int", varDecl->type);
    CHECK_EQUAL ("x", varDecl->name);
    CHECK_TRUE (varDecl->init == nullptr);
}

TEST (parser_test_group, test_parse_var_decl_statement_with_init)
{
    createParser ("float y = 3.14;");
    
    auto stmt = pt->parseVarDeclStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto varDecl = dynamic_cast<VarDecl*> (stmt.get());
    CHECK_TRUE (varDecl != nullptr);
    CHECK_EQUAL ("float", varDecl->type);
    CHECK_EQUAL ("y", varDecl->name);
    CHECK_TRUE (varDecl->init != nullptr);
}

TEST (parser_test_group, test_parse_statement_expression)
{
    createParser ("x + y;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto exprStmt = dynamic_cast<ExpressionStmt*> (stmt.get());
    CHECK_TRUE (exprStmt != nullptr);
    CHECK_TRUE (exprStmt->expr != nullptr);
}

TEST (parser_test_group, test_parse_statement_var_decl)
{
    createParser ("int z = 42;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto varDecl = dynamic_cast<VarDecl*> (stmt.get());
    CHECK_TRUE (varDecl != nullptr);
    CHECK_EQUAL ("int", varDecl->type);
}

TEST (parser_test_group, test_parse_break_via_statement)
{
    createParser ("break;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    CHECK_TRUE (dynamic_cast<BreakStmt*>(stmt.get()) != nullptr);
}

TEST (parser_test_group, test_parse_continue_via_statement)
{
    createParser ("continue;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    CHECK_TRUE (dynamic_cast<ContinueStmt*>(stmt.get()) != nullptr);
}

TEST (parser_test_group, test_parse_return_with_value)
{
    createParser ("return 42;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto returnStmt = dynamic_cast<ReturnStmt*> (stmt.get());
    CHECK_TRUE (returnStmt != nullptr);
    CHECK_TRUE (returnStmt->value != nullptr);
}

TEST (parser_test_group, test_parse_return_void)
{
    createParser ("return;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto returnStmt = dynamic_cast<ReturnStmt*> (stmt.get());
    CHECK_TRUE (returnStmt != nullptr);
    CHECK_TRUE (returnStmt->value == nullptr);
}

TEST (parser_test_group, test_parse_if_with_no_else_branch)
{
    createParser ("if (x > 0) x = 1;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto ifStmt = dynamic_cast<IfStmt*> (stmt.get());
    CHECK_TRUE (ifStmt != nullptr);
    CHECK_TRUE (ifStmt->condition != nullptr);
    CHECK_TRUE (ifStmt->thenBranch != nullptr);
    CHECK_TRUE (ifStmt->elseBranch == nullptr);
}

TEST (parser_test_group, test_parse_if_with_else)
{
    createParser ("if (x > 0) x = 1; else x = -1;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto ifStmt = dynamic_cast<IfStmt*> (stmt.get());
    CHECK_TRUE (ifStmt != nullptr);
    CHECK_TRUE (ifStmt->condition != nullptr);
    CHECK_TRUE (ifStmt->thenBranch != nullptr);
    CHECK_TRUE (ifStmt->elseBranch != nullptr);
}

TEST (parser_test_group, test_pars_while_trivial)
{
    createParser ("while (true) break;");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto whileStmt = dynamic_cast<WhileStmt*> (stmt.get());
    CHECK_TRUE (whileStmt != nullptr);
    CHECK_TRUE (whileStmt->condition != nullptr);
    CHECK_TRUE (whileStmt->body != nullptr);
}

TEST (parser_test_group, test_parse_do_while_trivial)
{
    createParser ("do { x++; } while (x < 10);");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto doWhileStmt = dynamic_cast<DoWhileStmt*> (stmt.get());
    CHECK_TRUE (doWhileStmt != nullptr);
    CHECK_TRUE (doWhileStmt->body != nullptr);
    CHECK_TRUE (doWhileStmt->condition != nullptr);
}

TEST (parser_test_group, test_parse_for_simple)
{
    createParser ("for (i = 0; i < 10; i++) { sum += i; }");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto forStmt = dynamic_cast<ForStmt*> (stmt.get());
    CHECK_TRUE (forStmt != nullptr);
    CHECK_TRUE (forStmt->init != nullptr);
    CHECK_TRUE (forStmt->condition != nullptr);
    CHECK_TRUE (forStmt->update != nullptr);
    CHECK_TRUE (forStmt->body != nullptr);
}

TEST (parser_test_group, test_parse_for_empty_head)
{
    createParser ("for (;;) { break; }");
    
    auto stmt = pt->parseStatement ();
    CHECK_TRUE (stmt != nullptr);
    auto forStmt = dynamic_cast<ForStmt*> (stmt.get());
    CHECK_TRUE (forStmt != nullptr);
    CHECK_TRUE (forStmt->init == nullptr);
    CHECK_TRUE (forStmt->condition == nullptr);
    CHECK_TRUE (forStmt->update == nullptr);
    CHECK_TRUE (forStmt->body != nullptr);
}

TEST (parser_test_group, test_parse_block_empty_block)
{
    createParser ("{}");
    
    auto block = pt->parseBlock ();
    CHECK_TRUE (block != nullptr);
    CHECK_EQUAL (0, block->statements.size());
}

TEST (parser_test_group, test_parse_block_with_statements)
{
    createParser ("{ int x = 0; x++; return x; }");
    
    auto block = pt->parseBlock ();
    CHECK_TRUE (block != nullptr);
    CHECK_EQUAL (3, block->statements.size());
}

TEST (parser_test_group, test_parse_function_simple)
{
    createParser ("int add(int a, int b) { return a + b; }");
    
    auto func = pt->parseFunction ();
    CHECK_TRUE (func != nullptr);
    CHECK_EQUAL ("int", func->returnType);
    CHECK_EQUAL ("add", func->name);
    CHECK_EQUAL (2, func->params.size());
    CHECK_TRUE (func->body != nullptr);
}

TEST (parser_test_group, test_parse_function_with_no_params)
{
    createParser ("void print() { }");
    
    auto func = pt->parseFunction ();
    CHECK_TRUE (func != nullptr);
    CHECK_EQUAL ("void", func->returnType);
    CHECK_EQUAL ("print", func->name);
    CHECK_EQUAL (0, func->params.size());
    CHECK_TRUE (func->body != nullptr);
}

TEST (parser_test_group, test_parse_program_empty_program)
{
    createParser ("");
    
    auto program = parser->parseProgram ();
    CHECK_TRUE (program != nullptr);
    CHECK_EQUAL (0, program->functions.size());
}

TEST (parser_test_group, test_parseProgram_multiple_functions)
{
    createParser ("void init() {} int compute() { return 42; } void cleanup() {}");
    
    auto program = parser->parseProgram ();
    CHECK_TRUE (program != nullptr);
    CHECK_EQUAL (3, program->functions.size());
}

TEST (parser_test_group, test_operator_precedence)
{
    createParser ("a + b * c");
    
    // Must be parsed as a + (b * c)
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("+", binExpr->op);
}

TEST (parser_test_group, test_nested_expressions)
{
    createParser ("(a + b) * (c - d)");
    
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("*", binExpr->op);
}

TEST (parser_test_group, test_complex_assignment)
{
    createParser ("x = y = z = 0");
    
    // Must be parsed as x = (y = (z = 0))
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("=", binExpr->op);
}

TEST (parser_test_group, test_logical_expression)
{
    createParser ("a > 0 && b < 10 || c == 5");
    
    // Must be parsed as ((a > 0) && (b < 10)) || (c == 5))
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("||", binExpr->op); // || имеет самый низкий приоритет
}

TEST (parser_test_group, test_unexpected_token)
{
    createParser ("*");
    
    CHECK_THROWS (std::runtime_error, pt->parseExpression());
}

TEST (parser_test_group, test_missing_semicolon)
{
    createParser ("x = 5");
    
    CHECK_THROWS (std::runtime_error, pt->parseStatement());
}

TEST (parser_test_group, test_missing_parenthesis)
{
    createParser ("if x > 0) { }");
    
    CHECK_THROWS (std::runtime_error, pt->parseStatement());
}

TEST (parser_test_group, test_incomplete_expression)
{
    createParser ("x + ");
    
    CHECK_THROWS (std::runtime_error, pt->parseExpression());
}

TEST (parser_test_group, test_mixed_statements)
{
    createParser ("{ int i = 0; while (i < 10) { i++; } return i; }");
    
    auto block = pt->parseBlock ();
    CHECK_TRUE (block != nullptr);
    CHECK_EQUAL (3, block->statements.size());
}

TEST (parser_test_group, test_compound_assignment_operators)
{
    std::vector<std::string> operators = {"+=", "-=", "*=", "/=", "%="};
    
    for  (const auto& op : operators) {
        createParser ("x " + op + " 5;");
        
        auto stmt = pt->parseStatement ();
        CHECK_TRUE (stmt != nullptr);

        auto exprStmt = dynamic_cast<ExpressionStmt*> (stmt.get());
        CHECK_TRUE (exprStmt != nullptr);

        auto binExpr = dynamic_cast<BinaryExpr*> (exprStmt->expr.get());
        CHECK_TRUE (binExpr != nullptr);

        CHECK_EQUAL (op, binExpr->op);
    }
}

TEST (parser_test_group, test_comparison_operators)
{
    std::vector<std::string> operators = {"<", ">", "<=", ">=", "==", "!="};
    
    for  (const auto& op : operators) {
        createParser ("a " + op + " b");
        
        auto expr = pt->parseExpression ();
        CHECK_TRUE (expr != nullptr);
        auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
        CHECK_TRUE (binExpr != nullptr);
        CHECK_EQUAL (op, binExpr->op);
    }
}

TEST (parser_test_group, test_arithmetic_operators)
{
    std::vector<std::string> operators = {"+", "-", "*", "/", "%"};
    
    for (const auto& op : operators) {
        createParser ("a " + op + " b");
        
        auto expr = pt->parseExpression ();
        CHECK_TRUE (expr != nullptr);
        auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
        CHECK_TRUE (binExpr != nullptr);
        CHECK_EQUAL (op, binExpr->op);
    }
}

TEST (parser_test_group, test_nested_blocks)
{
    createParser ("{{{{{}}}}}");
    
    auto block = pt->parseBlock ();
    CHECK_TRUE (block != nullptr);
    
    // The external unit consists of one internal unit
    CHECK_EQUAL (1, block->statements.size());
}

TEST (parser_test_group, test_assignment_chain)
{
    createParser ("a = b = c = 42");
    
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);

    // Check if this is a chain of assignments
    auto binExpr = dynamic_cast<BinaryExpr*> (expr.get());
    CHECK_TRUE (binExpr != nullptr);
    CHECK_EQUAL ("=", binExpr->op);
}

TEST (parser_test_group, test_operator_priority)
{
    createParser ("!a && b || c * d + e");
    
    // Must be parsed as ((!a && b) || ((c * d) + e))
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);
}

TEST (parser_test_group, test_prefix_postfix)
{
    createParser ("++x--");
    
    // Must be parsed as ++ (x--)
    auto expr = pt->parseExpression ();
    CHECK_TRUE (expr != nullptr);

    auto unary = dynamic_cast<UnaryExpr*> (expr.get());
    CHECK_TRUE (unary != nullptr);
    CHECK_EQUAL ("++", unary->op);
}
