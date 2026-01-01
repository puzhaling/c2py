#include "control_flow_translator.h"
#include "expr_translator.h"
#include <algorithm>
#include <cctype>
#include <unordered_set>
#include <sstream>

static const std::unordered_set<std::string> TYPES = {
    "int", "float", "double", "char", "bool"
};

static inline std::string trim(const std::string &s) {
    size_t a = 0;
    while (a < s.size() && std::isspace((unsigned char)s[a])) a++;
    size_t b = s.size();
    while (b > a && std::isspace((unsigned char)s[b-1])) b--;
    return s.substr(a, b-a);
}

std::string ControlFlowTranslator::indentStr() const {
    int safeIndent = indent < 0 ? 0 : indent;
    return std::string(safeIndent * 4, ' ');
}

// find matching separator: openSym and closeSym are strings like "(", ")" or "{", "}"
// start is index of the opening token (or at least the search starts there)
size_t ControlFlowTranslator::findMatching(const std::vector<Token>& toks, size_t start, const std::string& openSym, const std::string& closeSym) const {
    if (start >= toks.size()) return toks.size();
    size_t i = start;
    int depth = 0;
    for (; i < toks.size(); ++i) {
        const std::string &lx = toks[i].lexeme;
        if (lx == openSym) {
            depth++;
            // if open at start and we want to find matching closing, continue
        } else if (lx == closeSym) {
            depth--;
            if (depth == 0) return i;
        }
    }
    return toks.size(); // not found
}

std::vector<Token> ControlFlowTranslator::sliceTokens(const std::vector<Token>& toks, size_t a, size_t b) const {
    if (a >= b || a >= toks.size()) return {};
    b = std::min(b, toks.size());
    return std::vector<Token>(toks.begin() + a, toks.begin() + b);
}

std::string ControlFlowTranslator::translate(const std::vector<Token>& tokens) {
    ExpressionTranslator expr;
    std::string out;
    size_t pos = 0;
    const auto& toks = tokens;

    // translate a simple statement: consume tokens until semicolon (inclusive),
    // strip leading type keywords and trailing semicolons, return indented line + newline.
    auto translateSimpleStatement = [&](size_t &p) -> std::string {
        size_t s = p;
        size_t e = s;
        while (e < toks.size() && !(toks[e].type == TokenType::Separator && toks[e].lexeme == ";")) e++;
        if (e < toks.size()) e++; // include semicolon
        auto slice = sliceTokens(toks, s, e);
        p = e;

        // remove leading type token if present
        size_t startIdx = 0;
        if (!slice.empty() && slice[0].type == TokenType::Keyword && TYPES.count(slice[0].lexeme)) startIdx = 1;
        std::vector<Token> cleaned(slice.begin() + startIdx, slice.end());

        std::string r = expr.translate(cleaned);
        // remove all semicolons and trim trailing spaces
        r.erase(std::remove(r.begin(), r.end(), ';'), r.end());
        r = trim(r);
        if (r.empty()) return std::string();
        return indentStr() + r + "\n";
    };

    // translate block by creating inner translator with increased indent
    auto translateBlock = [&](size_t start, size_t end, int newIndent) -> std::string {
        ControlFlowTranslator inner;
        inner.indent = newIndent;
        auto slice = sliceTokens(toks, start, end);
        return inner.translate(slice);
    };

    while (pos < toks.size() && toks[pos].type != TokenType::EndOfFile) {
        const Token& t = toks[pos];

        // --- function (including main)
        // cases: "int foo ( ... ) { ... }"  OR "foo ( ... ) { ... }"
        if ((t.type == TokenType::Keyword && TYPES.count(t.lexeme) && pos + 2 < toks.size() && toks[pos+1].type == TokenType::Identifier && toks[pos+2].lexeme == "(")
         || (t.type == TokenType::Identifier && pos + 1 < toks.size() && toks[pos+1].lexeme == "(")) {
            size_t idPos = (t.type == TokenType::Keyword) ? pos + 1 : pos;
            std::string fname = toks[idPos].lexeme;
            size_t parOpen = idPos + 1; // position of '('
            size_t parClose = findMatching(toks, parOpen, "(", ")");
            size_t after = (parClose < toks.size()) ? parClose + 1 : parClose;
            if (after < toks.size() && toks[after].lexeme == "{") {
                // it's a function definition
                // collect params identifiers
                std::vector<std::string> params;
                for (size_t i = parOpen + 1; i < parClose; ++i) {
                    if (toks[i].type == TokenType::Identifier) params.push_back(toks[i].lexeme);
                }
                if (fname == "main") {
                    out += "if __name__ == \"__main__\":\n";
                } else {
                    out += "def " + fname + "(";
                    for (size_t k = 0; k < params.size(); ++k) {
                        out += params[k];
                        if (k + 1 < params.size()) out += ", ";
                    }
                    out += "):\n";
                }
                size_t braceOpen = after;
                size_t braceClose = findMatching(toks, braceOpen, "{", "}");
                size_t bodyStart = braceOpen + 1;
                size_t bodyEnd = (braceClose == toks.size()) ? toks.size() : braceClose;
                out += translateBlock(bodyStart, bodyEnd, indent + 1);
                pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
                continue;
            }
        }

        // --- if
        if (t.type == TokenType::Keyword && t.lexeme == "if") {
            pos++;
            // skip '(' if present
            if (pos < toks.size() && toks[pos].lexeme == "(") { /* pos stays at '(' */ }
            size_t parOpen = (pos < toks.size() && toks[pos].lexeme == "(") ? pos : pos - 1;
            size_t parClose = findMatching(toks, parOpen, "(", ")");
            size_t condStart = parOpen + 1;
            size_t condEnd = (parClose == toks.size()) ? parOpen : parClose;
            auto condSlice = sliceTokens(toks, condStart, condEnd);
            std::string cond = expr.translate(condSlice);
            cond.erase(std::remove(cond.begin(), cond.end(), ';'), cond.end());
            cond = trim(cond);
            out += indentStr() + "if " + cond + ":\n";
            pos = parClose + 1;
            // body
            if (pos < toks.size() && toks[pos].lexeme == "{") {
                size_t braceClose = findMatching(toks, pos, "{", "}");
                out += translateBlock(pos + 1, braceClose, indent + 1);
                pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
            } else {
                indent++;
                out += translateSimpleStatement(pos);
                indent--;
            }
            continue;
        }

        // --- else / else if (elif)
        if (t.type == TokenType::Keyword && t.lexeme == "else") {
            if (pos + 1 < toks.size() && toks[pos+1].type == TokenType::Keyword && toks[pos+1].lexeme == "if") {
                // else if -> elif
                pos += 2; // skip 'else' 'if', now at maybe '('
                if (pos < toks.size() && toks[pos].lexeme == "(") { /* fine */ }
                size_t parOpen = (pos < toks.size() && toks[pos].lexeme == "(") ? pos : pos - 1;
                size_t parClose = findMatching(toks, parOpen, "(", ")");
                auto condSlice = sliceTokens(toks, parOpen + 1, parClose);
                std::string cond = expr.translate(condSlice);
                cond.erase(std::remove(cond.begin(), cond.end(), ';'), cond.end());
                cond = trim(cond);
                out += indentStr() + "elif " + cond + ":\n";
                pos = parClose + 1;
                if (pos < toks.size() && toks[pos].lexeme == "{") {
                    size_t braceClose = findMatching(toks, pos, "{", "}");
                    out += translateBlock(pos + 1, braceClose, indent + 1);
                    pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
                } else {
                    indent++;
                    out += translateSimpleStatement(pos);
                    indent--;
                }
                continue;
            } else {
                // plain else
                pos++;
                out += indentStr() + "else:\n";
                if (pos < toks.size() && toks[pos].lexeme == "{") {
                    size_t braceClose = findMatching(toks, pos, "{", "}");
                    out += translateBlock(pos + 1, braceClose, indent + 1);
                    pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
                } else {
                    indent++;
                    out += translateSimpleStatement(pos);
                    indent--;
                }
                continue;
            }
        }

        // --- while
        if (t.type == TokenType::Keyword && t.lexeme == "while") {
            pos++;
            if (pos < toks.size() && toks[pos].lexeme == "(") { /* ok */ }
            size_t parOpen = (pos < toks.size() && toks[pos].lexeme == "(") ? pos : pos - 1;
            size_t parClose = findMatching(toks, parOpen, "(", ")");
            auto condSlice = sliceTokens(toks, parOpen + 1, parClose);
            std::string cond = expr.translate(condSlice);
            cond.erase(std::remove(cond.begin(), cond.end(), ';'), cond.end());
            cond = trim(cond);
            out += indentStr() + "while " + cond + ":\n";
            pos = parClose + 1;
            if (pos < toks.size() && toks[pos].lexeme == "{") {
                size_t braceClose = findMatching(toks, pos, "{", "}");
                out += translateBlock(pos + 1, braceClose, indent + 1);
                pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
            } else {
                indent++;
                out += translateSimpleStatement(pos);
                indent--;
            }
            continue;
        }

        // --- do { .. } while(cond);
        if (t.type == TokenType::Keyword && t.lexeme == "do") {
            pos++;
            std::string body;
            if (pos < toks.size() && toks[pos].lexeme == "{") {
                size_t braceClose = findMatching(toks, pos, "{", "}");
                body = translateBlock(pos + 1, braceClose, indent + 1);
                pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
            } else {
                indent++;
                body = translateSimpleStatement(pos);
                indent--;
            }
            // expect while
            if (pos < toks.size() && toks[pos].type == TokenType::Keyword && toks[pos].lexeme == "while") {
                pos++;
                if (pos < toks.size() && toks[pos].lexeme == "(") { /* ok */ }
                size_t parOpen = (pos < toks.size() && toks[pos].lexeme == "(") ? pos : pos - 1;
                size_t parClose = findMatching(toks, parOpen, "(", ")");
                auto condSlice = sliceTokens(toks, parOpen + 1, parClose);
                std::string cond = expr.translate(condSlice);
                cond.erase(std::remove(cond.begin(), cond.end(), ';'), cond.end());
                cond = trim(cond);
                out += indentStr() + "while True:\n";
                out += body;
                out += indentStr() + "    if not (" + cond + "):\n";
                out += indentStr() + "        break\n";
                pos = parClose + 1;
                if (pos < toks.size() && toks[pos].lexeme == ";") pos++;
                continue;
            } else {
                out += indentStr() + "while True:\n" + body;
                continue;
            }
        }

        // --- for (init; cond; upd)
        if (t.type == TokenType::Keyword && t.lexeme == "for") {
            pos++;
            if (pos < toks.size() && toks[pos].lexeme == "(") { /* ok */ }
            size_t parOpen = (pos < toks.size() && toks[pos].lexeme == "(") ? pos : pos - 1;
            size_t parClose = findMatching(toks, parOpen, "(", ")");
            // split by semicolons
            size_t a = parOpen + 1;
            size_t semi1 = a;
            while (semi1 < parClose && !(toks[semi1].type == TokenType::Separator && toks[semi1].lexeme == ";")) semi1++;
            size_t b = semi1 + 1;
            size_t semi2 = b;
            while (semi2 < parClose && !(toks[semi2].type == TokenType::Separator && toks[semi2].lexeme == ";")) semi2++;
            size_t c = semi2 + 1;

            auto initSlice = sliceTokens(toks, a, semi1);
            auto condSlice = sliceTokens(toks, b, semi2);
            auto updSlice  = sliceTokens(toks, c, parClose);

            std::string initS = expr.translate(initSlice);
            std::string condS = expr.translate(condSlice);
            std::string updS  = expr.translate(updSlice);

            // clean strings
            initS.erase(std::remove(initS.begin(), initS.end(), ';'), initS.end());
            condS.erase(std::remove(condS.begin(), condS.end(), ';'), condS.end());
            updS.erase(std::remove(updS.begin(), updS.end(), ';'), updS.end());
            initS = trim(initS);
            condS = trim(condS);
            updS  = trim(updS);

            // try to detect simple i = START; i < END; i++ pattern
            bool usedRange = false;
            std::string forHeader;
            if (!initS.empty() && !condS.empty() && !updS.empty()) {
                // init like "i = 0"
                size_t eq = initS.find('=');
                if (eq != std::string::npos) {
                    std::string var = trim(initS.substr(0, eq));
                    std::string start = trim(initS.substr(eq + 1));
                    // cond like "i < N"
                    size_t lt = condS.find('<');
                    if (lt != std::string::npos) {
                        std::string condVar = trim(condS.substr(0, lt));
                        std::string end = trim(condS.substr(lt + 1));
                        // upd contains var and either "++" or "+=" or var += 1
                        if (updS.find(var) != std::string::npos && (updS.find("++") != std::string::npos || updS.find("+=") != std::string::npos)) {
                            // produce for var in range(start, end):
                            forHeader = indentStr() + "for " + var + " in range(" + start + ", " + end + "):\n";
                            usedRange = true;
                        }
                    }
                }
            }

            if (!usedRange) {
                // fallback: emit init then while cond
                if (!initS.empty()) out += indentStr() + initS + "\n";
                out += indentStr() + "while " + (condS.empty() ? "True" : condS) + ":\n";
            } else {
                out += forHeader;
            }

            pos = parClose + 1;
            // body
            if (pos < toks.size() && toks[pos].lexeme == "{") {
                size_t braceClose = findMatching(toks, pos, "{", "}");
                out += translateBlock(pos + 1, braceClose, indent + 1);
                pos = (braceClose == toks.size()) ? toks.size() : braceClose + 1;
            } else {
                indent++;
                out += translateSimpleStatement(pos);
                indent--;
            }

            // if we used while-emulation, append upd after body
            if (!usedRange && !updS.empty()) {
                out += indentStr() + updS + "\n";
            } else if (usedRange) {
                // nothing to append
            }

            continue;
        }

        // --- return / break / continue (standalone)
        if (t.type == TokenType::Keyword && (t.lexeme == "return" || t.lexeme == "break" || t.lexeme == "continue")) {
            out += translateSimpleStatement(pos);
            continue;
        }

        // --- semicolon safety
        if (t.type == TokenType::Separator && t.lexeme == ";") {
            pos++;
            continue;
        }

        // --- fallback (expression / assignment / call)
        out += translateSimpleStatement(pos);
    }

    return out;
}
