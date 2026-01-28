#include "parser.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <memory>

// --- constructor
Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)), pos(0) {
    // initial scope already pushed by SymbolTable ctor
}

// --- token navigation
const Token& Parser::peek() const {
    if (pos < tokens.size()) return tokens[pos];
    static Token eofTok{TokenType::EndOfFile, "", 0, 0};
    return eofTok;
}

const Token& Parser::previous() const {
    if (pos == 0) return peek(); // defensive, but previous() should be called only after advance()/match()
    return tokens[pos-1];
}

const Token& Parser::advance() {
    if (!atEnd()) ++pos;
    return previous();
}

bool Parser::atEnd() const {
    return pos >= tokens.size() || tokens[pos].type == TokenType::EndOfFile;
}

bool Parser::check(TokenType type, const std::string& lexeme) const {
    if (atEnd()) return false;
    if (tokens[pos].type != type) return false;
    return lexeme.empty() || tokens[pos].lexeme == lexeme;
}

bool Parser::match(TokenType type, const std::string& lexeme) {
    if (check(type, lexeme)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& lexeme) {
    if (!match(type, lexeme)) {
        std::ostringstream ss;
        ss << "Syntax error at " << tokenLocation()
           << ": expected '" << (lexeme.empty()? "<token>" : lexeme)
           << "', got '" << peek().lexeme << "'";
        throw std::runtime_error(ss.str());
    }
}

std::string Parser::tokenLocation() const {
    if (atEnd()) return "<EOF>";
    std::ostringstream ss;
    ss << "line " << peek().line << " col " << peek().column;
    return ss.str();
}

// --- parseProgram
ProgramPtr Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (!atEnd()) {
        if (match(TokenType::Separator, ";")) continue;
        auto func = parseFunction();
        if (func) {
            ASTNode* raw = func.get();
            // register top-level function name in global scope
            symbols.declare(func->name, raw);
            program->functions.push_back(std::move(func));
        } else break;
    }
    return program;
}

// --- parseFunction
FuncPtr Parser::parseFunction() {
    std::string retType;
    if (check(TokenType::Keyword)) { retType = peek().lexeme; advance(); }

    if (!check(TokenType::Identifier)) {
        throw std::runtime_error("Expected function name at " + tokenLocation());
    }
    std::string name = peek().lexeme; advance();

    expect(TokenType::Separator, "(");
    std::vector<std::pair<std::string,std::string>> params;
    if (!check(TokenType::Separator, ")")) {
        while (true) {
            if (!check(TokenType::Keyword)) throw std::runtime_error("Expected parameter type at " + tokenLocation());
            std::string ptype = peek().lexeme; advance();
            if (!check(TokenType::Identifier)) throw std::runtime_error("Expected parameter name at " + tokenLocation());
            std::string pname = peek().lexeme; advance();
            params.emplace_back(ptype, pname);
            if (match(TokenType::Separator, ",")) continue;
            break;
        }
    }
    expect(TokenType::Separator, ")");

    if (!check(TokenType::Separator, "{")) throw std::runtime_error("Expected '{' at " + tokenLocation());

    auto func = std::make_unique<FunctionDecl>();
    func->returnType = retType;
    func->name = name;
    func->params = params;

    // Function scope: params declared here so they are visible in function body
    symbols.pushScope();
    for (const auto &p : params) {
        symbols.declare(p.second, func.get());
    }

    // parse body (parseBlock will push/pop an inner block scope)
    func->body = parseBlock();

    symbols.popScope();
    return func;
}

// --- parseBlock
std::unique_ptr<BlockStmt> Parser::parseBlock() {
    expect(TokenType::Separator, "{");
    auto block = std::make_unique<BlockStmt>();
    symbols.pushScope();
    while (!check(TokenType::Separator, "}") && !atEnd()) {
        block->statements.push_back(parseStatement());
    }
    expect(TokenType::Separator, "}");
    symbols.popScope();
    return block;
}

// --- parseStatement
StmtPtr Parser::parseStatement() {
    if (match(TokenType::Keyword, "if")) return parseIf();
    if (match(TokenType::Keyword, "while")) return parseWhile();
    if (match(TokenType::Keyword, "do")) return parseDoWhile();
    if (match(TokenType::Keyword, "for")) return parseFor();
    if (match(TokenType::Keyword, "return")) return parseReturn();
    if (match(TokenType::Keyword, "break")) return parseBreak();
    if (match(TokenType::Keyword, "continue")) return parseContinue();
    if (check(TokenType::Keyword)) return parseVarDeclStatement();
    if (match(TokenType::Separator, "{")) { --pos; return parseBlock(); }

    auto expr = parseExpression();
    expect(TokenType::Separator, ";");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

// --- if-else if-else (recursive else-if handling)
StmtPtr Parser::parseIf() {
    expect(TokenType::Separator, "(");
    auto cond = parseExpression();
    expect(TokenType::Separator, ")");

    auto thenStmt = parseStatement();
    std::unique_ptr<Statement> elseStmt = nullptr;

    if (match(TokenType::Keyword, "else")) {
        if (match(TokenType::Keyword, "if")) {
            // 'else if' -> recursively parse another if and attach as else branch
            elseStmt = parseIf();
        } else {
            elseStmt = parseStatement();
        }
    }

    auto node = std::make_unique<IfStmt>();
    node->condition = std::move(cond);
    node->thenBranch = std::move(thenStmt);
    node->elseBranch = std::move(elseStmt);
    return node;
}

// --- while
StmtPtr Parser::parseWhile() {
    expect(TokenType::Separator, "(");
    auto cond = parseExpression();
    expect(TokenType::Separator, ")");
    auto body = parseStatement();
    auto node = std::make_unique<WhileStmt>();
    node->condition = std::move(cond);
    node->body = std::move(body);
    return node;
}

// --- do-while
StmtPtr Parser::parseDoWhile() {
    auto body = parseStatement();
    expect(TokenType::Keyword, "while");
    expect(TokenType::Separator, "(");
    auto cond = parseExpression();
    expect(TokenType::Separator, ")");
    expect(TokenType::Separator, ";");

    auto node = std::make_unique<DoWhileStmt>();
    node->body = std::move(body);
    node->condition = std::move(cond);
    return node;
}

// --- for
StmtPtr Parser::parseFor() {
    expect(TokenType::Separator, "(");

    std::unique_ptr<Statement> initStmt = nullptr;
    if (!check(TokenType::Separator, ";")) {
        if (check(TokenType::Keyword)) initStmt = parseVarDeclStatement();
        else {
            auto e = parseExpression();
            expect(TokenType::Separator, ";");
            initStmt = std::make_unique<ExpressionStmt>(std::move(e));
        }
    } else { expect(TokenType::Separator, ";"); }

    std::unique_ptr<Expression> condExpr = nullptr;
    if (!check(TokenType::Separator, ";")) condExpr = parseExpression();
    expect(TokenType::Separator, ";");

    std::unique_ptr<Expression> updExpr = nullptr;
    if (!check(TokenType::Separator, ")")) updExpr = parseExpression();
    expect(TokenType::Separator, ")");

    auto body = parseStatement();

    auto node = std::make_unique<ForStmt>();
    node->init = std::move(initStmt);
    node->condition = std::move(condExpr);
    node->update = std::move(updExpr);
    node->body = std::move(body);
    return node;
}

// --- return / break / continue / var decl
StmtPtr Parser::parseReturn() {
    std::unique_ptr<Expression> val = nullptr;
    if (!check(TokenType::Separator, ";")) val = parseExpression();
    expect(TokenType::Separator, ";");
    auto node = std::make_unique<ReturnStmt>();
    node->value = std::move(val);
    return node;
}

StmtPtr Parser::parseBreak() { expect(TokenType::Separator, ";"); return std::make_unique<BreakStmt>(); }
StmtPtr Parser::parseContinue() { expect(TokenType::Separator, ";"); return std::make_unique<ContinueStmt>(); }

StmtPtr Parser::parseVarDeclStatement() {
    std::string type = peek().lexeme; advance();
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected identifier after type at " + tokenLocation());
    std::string name = peek().lexeme; advance();

    std::unique_ptr<Expression> init = nullptr;
    if (match(TokenType::Operator, "=")) init = parseExpression();
    expect(TokenType::Separator, ";");

    auto node = std::make_unique<VarDecl>(type, name, std::move(init));
    symbols.declare(name, node.get());
    return node;
}

// --- expressions (precedence climbing)
ExprPtr Parser::parseExpression() { return parseAssignment(); }

ExprPtr Parser::parseAssignment() {
    auto left = parseLogicalOr();

    // check for assignment operators (including compound)
    if (check(TokenType::Operator)) {
        std::string op = peek().lexeme;
        if (op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=") {
            advance(); // consume operator
            auto right = parseAssignment();
            return std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        }
    }
    return left;
}

ExprPtr Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::Operator, "||")) {
        std::string op = previous().lexeme;
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::Operator, "&&")) {
        std::string op = previous().lexeme;
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseEquality() {
    auto expr = parseRelational();
    while (match(TokenType::Operator, "==") || match(TokenType::Operator, "!=")) {
        std::string op = previous().lexeme;
        auto right = parseRelational();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseRelational() {
    auto expr = parseAdditive();
    while (match(TokenType::Operator, "<") || match(TokenType::Operator, ">") ||
           match(TokenType::Operator, "<=") || match(TokenType::Operator, ">=")) {
        std::string op = previous().lexeme;
        auto right = parseAdditive();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    while (match(TokenType::Operator, "+") || match(TokenType::Operator, "-")) {
        std::string op = previous().lexeme;
        auto right = parseMultiplicative();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseMultiplicative() {
    auto expr = parseUnary();
    while (match(TokenType::Operator, "*") || match(TokenType::Operator, "/") || match(TokenType::Operator, "%")) {
        std::string op = previous().lexeme;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

// --- unary (prefix) -> then postfix
ExprPtr Parser::parseUnary() {
    if (match(TokenType::Operator, "!") || match(TokenType::Operator, "-") ||
        match(TokenType::Operator, "++") || match(TokenType::Operator, "--")) {
        std::string op = previous().lexeme;
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return parsePostfix();
}

// --- postfix handling (primary then post-increment/decrement)
ExprPtr Parser::parsePostfix() {
    auto expr = parsePrimary();

    // post-increment / post-decrement
    while (match(TokenType::Operator, "++") || match(TokenType::Operator, "--")) {
        std::string op = previous().lexeme;
        expr = std::make_unique<UnaryExpr>(op, std::move(expr));
    }
    return expr;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        return std::make_unique<NumberExpr>(previous().lexeme);
    }

    if (match(TokenType::Identifier)) {
        std::string name = previous().lexeme;
        
        // Check if it's a function call
        if (check(TokenType::Separator, "(")) {
            advance(); // consume '('
            auto call = std::make_unique<CallExpr>(name);
            
            // Parse arguments
            if (!check(TokenType::Separator, ")")) {
                do {
                    call->args.push_back(parseExpression());
                } while (match(TokenType::Separator, ","));
            }
            
            expect(TokenType::Separator, ")");
            return call;
        }
        
        // Otherwise it's just an identifier
        auto id = std::make_unique<IdentifierExpr>(name);
        ASTNode* decl = symbols.lookup(id->name);
        if (decl) id->declaration = decl;
        return id;
    }

    if (match(TokenType::Separator, "(")) {
        auto expr = parseExpression();
        expect(TokenType::Separator, ")");
        return expr;
    }

    std::ostringstream ss;
    ss << "Unexpected token in expression at " << tokenLocation() << ": '" << peek().lexeme << "'";
    throw std::runtime_error(ss.str());
}
