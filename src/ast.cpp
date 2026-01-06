#include "ast.h"
#include <sstream>
#include <iostream>

static void indent(std::ostream &os, int n) {
    for (int i = 0; i < n; ++i) os << "  ";
}

static void printExpr(std::ostream &os, const Expression* e, int lvl);

static void printStmt(std::ostream &os, const Statement* s, int lvl) {
    if (!s) { indent(os, lvl); os << "<null-stmt>\n"; return; }

    if (auto es = dynamic_cast<const ExpressionStmt*>(s)) {
        indent(os, lvl); os << "ExpressionStmt:\n";
        printExpr(os, es->expr.get(), lvl+1);
        return;
    }
    if (auto vd = dynamic_cast<const VarDecl*>(s)) {
        indent(os, lvl); os << "VarDecl: " << vd->type << " " << vd->name << "\n";
        if (vd->init) printExpr(os, vd->init.get(), lvl+1);
        return;
    }
    if (auto bs = dynamic_cast<const BlockStmt*>(s)) {
        indent(os, lvl); os << "BlockStmt:\n";
        for (const auto &st : bs->statements) printStmt(os, st.get(), lvl+1);
        return;
    }
    if (auto is = dynamic_cast<const IfStmt*>(s)) {
        indent(os, lvl); os << "IfStmt:\n";
        indent(os, lvl+1); os << "Condition:\n"; printExpr(os, is->condition.get(), lvl+2);
        indent(os, lvl+1); os << "Then:\n"; printStmt(os, is->thenBranch.get(), lvl+2);
        if (is->elseBranch) { indent(os, lvl+1); os << "Else:\n"; printStmt(os, is->elseBranch.get(), lvl+2); }
        return;
    }
    if (auto ws = dynamic_cast<const WhileStmt*>(s)) {
        indent(os, lvl); os << "WhileStmt:\n";
        indent(os, lvl+1); os << "Condition:\n"; printExpr(os, ws->condition.get(), lvl+2);
        indent(os, lvl+1); os << "Body:\n"; printStmt(os, ws->body.get(), lvl+2);
        return;
    }
    if (auto dws = dynamic_cast<const DoWhileStmt*>(s)) {
        indent(os, lvl); os << "DoWhileStmt:\n";
        indent(os, lvl+1); os << "Body:\n"; printStmt(os, dws->body.get(), lvl+2);
        indent(os, lvl+1); os << "Condition:\n"; printExpr(os, dws->condition.get(), lvl+2);
        return;
    }
    if (auto fs = dynamic_cast<const ForStmt*>(s)) {
        indent(os, lvl); os << "ForStmt:\n";
        if (fs->init) { indent(os, lvl+1); os << "Init:\n"; printStmt(os, fs->init.get(), lvl+2); }
        if (fs->condition) { indent(os, lvl+1); os << "Condition:\n"; printExpr(os, fs->condition.get(), lvl+2); }
        if (fs->update) { indent(os, lvl+1); os << "Update:\n"; printExpr(os, fs->update.get(), lvl+2); }
        indent(os, lvl+1); os << "Body:\n"; printStmt(os, fs->body.get(), lvl+2);
        return;
    }
    if (auto rs = dynamic_cast<const ReturnStmt*>(s)) {
        indent(os, lvl); os << "ReturnStmt:\n";
        if (rs->value) printExpr(os, rs->value.get(), lvl+1);
        return;
    }
    if (dynamic_cast<const BreakStmt*>(s)) { indent(os, lvl); os << "BreakStmt\n"; return; }
    if (dynamic_cast<const ContinueStmt*>(s)) { indent(os, lvl); os << "ContinueStmt\n"; return; }

    indent(os, lvl); os << "UnknownStatement\n";
}

static void printExpr(std::ostream &os, const Expression* e, int lvl) {
    if (!e) { indent(os, lvl); os << "<null-expr>\n"; return; }

    if (auto ne = dynamic_cast<const NumberExpr*>(e)) {
        indent(os, lvl); os << "Number: " << ne->value << "\n"; return;
    }
    if (auto id = dynamic_cast<const IdentifierExpr*>(e)) {
        indent(os, lvl); os << "Identifier: " << id->name;
        if (id->declaration) os << " (decl)";
        os << "\n"; return;
    }
    if (auto ue = dynamic_cast<const UnaryExpr*>(e)) {
        indent(os, lvl); os << "Unary: " << ue->op << "\n";
        printExpr(os, ue->expr.get(), lvl+1); return;
    }
    if (auto be = dynamic_cast<const BinaryExpr*>(e)) {
        indent(os, lvl); os << "Binary: " << be->op << "\n";
        printExpr(os, be->lhs.get(), lvl+1);
        printExpr(os, be->rhs.get(), lvl+1);
        return;
    }

    indent(os, lvl); os << "UnknownExpression\n";
}

/* Публичная утилита для печати Program */
void printProgramAST(const Program* program, std::ostream &os) {
    if (!program) { os << "<null program>\n"; return; }
    os << "Program:\n";
    for (const auto &f : program->functions) {
        os << "Function: " << f->name << " returns " << f->returnType << "\n";
        os << " Params:\n";
        for (const auto &p : f->params) os << "  " << p.first << " " << p.second << "\n";
        os << " Body:\n";
        printStmt(os, f->body.get(), 1);
    }
}
