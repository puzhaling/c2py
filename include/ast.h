#pragma once
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <ostream>
#include <iostream>

// Базовые узлы AST
struct ASTNode {
    virtual ~ASTNode() = default;
    int line = 0;
    int column = 0;
};

/* ===== EXPRESSIONS ===== */
struct Expression : ASTNode { virtual ~Expression() = default; };

struct NumberExpr : Expression {
    std::string value;
    explicit NumberExpr(std::string v) : value(std::move(v)) {}
};

struct IdentifierExpr : Expression {
    std::string name;
    // ссылка на объявление (может быть nullptr, если не разрешено в момент парсинга)
    ASTNode* declaration = nullptr;
    explicit IdentifierExpr(std::string n) : name(std::move(n)) {}
};

struct UnaryExpr : Expression {
    std::string op;
    std::unique_ptr<Expression> expr;
    UnaryExpr(std::string o, std::unique_ptr<Expression> e)
        : op(std::move(o)), expr(std::move(e)) {}
};

struct BinaryExpr : Expression {
    std::string op;
    std::unique_ptr<Expression> lhs, rhs;
    BinaryExpr(std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(std::move(o)), lhs(std::move(l)), rhs(std::move(r)) {}
};

struct CallExpr : Expression {
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpr(std::string n) : name(std::move(n)) {}
};

/* ===== STATEMENTS ===== */
struct Statement : ASTNode { virtual ~Statement() = default; };

struct ExpressionStmt : Statement {
    std::unique_ptr<Expression> expr;
    explicit ExpressionStmt(std::unique_ptr<Expression> e) : expr(std::move(e)) {}
};

struct VarDecl : Statement {
    std::string type;
    std::string name;
    std::unique_ptr<Expression> init; // optional initializer
    VarDecl(std::string t, std::string n, std::unique_ptr<Expression> i = nullptr)
        : type(std::move(t)), name(std::move(n)), init(std::move(i)) {}
};

struct BlockStmt : Statement {
    std::vector<std::unique_ptr<Statement>> statements;
};

struct IfStmt : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch; // may be null
};

struct WhileStmt : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
};

struct DoWhileStmt : Statement {
    std::unique_ptr<Statement> body;
    std::unique_ptr<Expression> condition;
};

struct ForStmt : Statement {
    // init can be VarDecl or ExpressionStmt or nullptr
    std::unique_ptr<Statement> init;
    std::unique_ptr<Expression> condition; // may be nullptr (means True)
    std::unique_ptr<Expression> update;    // may be nullptr
    std::unique_ptr<Statement> body;
};

struct ReturnStmt : Statement {
    std::unique_ptr<Expression> value; // may be nullptr
};

struct BreakStmt : Statement {};
struct ContinueStmt : Statement {};

/* ===== TOP LEVEL ===== */

struct FunctionDecl : ASTNode {
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string,std::string>> params; // pair<type,name>
    std::unique_ptr<BlockStmt> body;
};

struct Program : ASTNode {
    std::vector<std::unique_ptr<FunctionDecl>> functions;
};

/* Utility type aliases */
using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;
using FuncPtr = std::unique_ptr<FunctionDecl>;
using ProgramPtr = std::unique_ptr<Program>;

/* Отладочная печать AST (прототип, реализация в src/ast.cpp) */
void printProgramAST(const Program* program, std::ostream& os = std::cout);
