<<<<<<< HEAD
// Header файл преобразователя выражений

#pragma once
#include "token.h"
#include <vector>
#include <string>

class ExpressionTranslator {
public:
    // Преобразование выражений
    std::string translate(const std::vector<Token>& tokens);
private:
    // Преобразование операторов
    std::string mapOperator(const std::string& op);
};
=======
// Header файл преобразователя выражений

#pragma once
#include "token.h"
#include <vector>
#include <string>

class ExpressionTranslator {
public:
    // Преобразование выражений
    std::string translate(const std::vector<Token>& tokens);
private:
    // Преобразование операторов
    std::string mapOperator(const std::string& op);
};
>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
