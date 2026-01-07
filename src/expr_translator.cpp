#include "expr_translator.h"

// Функция преобразования операторов
std::string ExpressionTranslator::mapOperator(const std::string& op) {
    if (op == "&&") return "and";
    if (op == "||") return "or";
    if (op == "!")  return "not";
    return op;
}
// Функция преобразования выражений
std::string ExpressionTranslator::translate(const std::vector<Token>& tokens) {
    std::string result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& t = tokens[i];
        if (t.type == TokenType::Operator) {
            if (t.lexeme == "++") {
                result += " += 1";
                continue;
            }
            if (t.lexeme == "--") {
                result += " -= 1";
                continue;
            }
            result += mapOperator(t.lexeme);
        }
        else {
            result += t.lexeme;
        }
        result += " ";
    }
    return result;
}
