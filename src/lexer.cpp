<<<<<<< HEAD
#include "lexer.h"
#include <cctype>
#include <stdexcept>

// Конструктор, начинающий анализ с начала кода
Lexer::Lexer(const std::string& src)
    : source(src), pos(0), line(1), column(1) {}
// Функция, возвращающая текущий символ
char Lexer::peek() const {
    return pos < source.size() ? source[pos] : '\0';
}
// Функция, считывающая символ и передвигающая указатель
char Lexer::get() {
    char c = peek();
    pos++;
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}
// Функция, проверяющая, закончился ли код
bool Lexer::eof() const {
    return pos >= source.size();
}
// Функция, пропускающая пробелы, табуляцию и перенос строк
void Lexer::skipWhitespace() {
    while (!eof() && std::isspace(peek())) get();
}
// Функция, считывающая токены из входного кода
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!eof()) {
        skipWhitespace();
        if (eof()) break;
        char c = peek();
        if (std::isalpha(c) || c == '_')
            tokens.push_back(readIdentifierOrKeyword());
        else if (std::isdigit(c))
            tokens.push_back(readNumber());
        else if (SEPARATORS.count(c))
            tokens.push_back(readSeparator());
        else
            tokens.push_back(readOperator());
    }
    // Обозначим конец файла после считывания кода
    tokens.emplace_back(TokenType::EndOfFile, "", line, column);
    return tokens;
}
// Функция чтения идентификатора/ключевого слова
Token Lexer::readIdentifierOrKeyword() {
    int startCol = column;
    std::string buf; // Буфер, в который записываем считанные символы
    // Считывание символов и чисел
    while (!eof() && (std::isalnum(peek()) || peek() == '_'))
        buf += get();
    // Проверяем, находится ли в буфере ключевое слово/идентификатор
    if (KEYWORDS.count(buf) || TYPES.count(buf))
        return {TokenType::Keyword, buf, line, startCol};
    return {TokenType::Identifier, buf, line, startCol};
}
// Функция чтения числа
Token Lexer::readNumber() {
    int startCol = column;
    std::string buf; // Буфер, в который записываем считанные символы
    // Считывание чисел
    while (!eof() && (std::isdigit(peek()) || peek() == '.'))
        buf += get();
    return {TokenType::Number, buf, line, startCol};
}
// Функция чтения оператора
Token Lexer::readOperator() {
    int startCol = column;
    std::string buf; // Буфер, в который записываем предполагаемый оператор
    // Считываем символы и проверяем, есть ли там оператор
    buf += get();
    std::string two = buf + peek();
    if (!eof()) {
        if (OPERATORS.count(two))
            buf += get();
    }
    if (!eof()) {
        std::string three = two + peek();
        if (OPERATORS.count(three))
            buf += get();
    }
    // Если не определили оператор, то выводим ошибку
    if (!OPERATORS.count(buf))
        throw std::runtime_error(
            "Неизвестный символ (" + std::to_string(line) + ":" +
            std::to_string(startCol) + ")");
    return {TokenType::Operator, buf, line, startCol};
}
// Функция чтения разделителя
Token Lexer::readSeparator() {
    char c = get();
    return {TokenType::Separator, std::string(1, c), line, column - 1};
}
=======
#include "lexer.h"
#include <cctype>
#include <stdexcept>

// Конструктор, начинающий анализ с начала кода
Lexer::Lexer(const std::string& src)
    : source(src), pos(0), line(1), column(1) {}
// Функция, возвращающая текущий символ
char Lexer::peek() const {
    return pos < source.size() ? source[pos] : '\0';
}
// Функция, считывающая символ и передвигающая указатель
char Lexer::get() {
    char c = peek();
    pos++;
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}
// Функция, проверяющая, закончился ли код
bool Lexer::eof() const {
    return pos >= source.size();
}
// Функция, пропускающая пробелы, табуляцию и перенос строк
void Lexer::skipWhitespace() {
    while (!eof() && std::isspace(peek())) get();
}
// Функция, считывающая токены из входного кода
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!eof()) {
        skipWhitespace();
        if (eof()) break;
        char c = peek();
        if (std::isalpha(c) || c == '_')
            tokens.push_back(readIdentifierOrKeyword());
        else if (std::isdigit(c))
            tokens.push_back(readNumber());
        else if (SEPARATORS.count(c))
            tokens.push_back(readSeparator());
        else
            tokens.push_back(readOperator());
    }
    // Обозначим конец файла после считывания кода
    tokens.emplace_back(TokenType::EndOfFile, "", line, column);
    return tokens;
}
// Функция чтения идентификатора/ключевого слова
Token Lexer::readIdentifierOrKeyword() {
    int startCol = column;
    std::string buf; // Буфер, в который записываем считанные символы
    // Считывание символов и чисел
    while (!eof() && (std::isalnum(peek()) || peek() == '_'))
        buf += get();
    // Проверяем, находится ли в буфере ключевое слово/идентификатор
    if (KEYWORDS.count(buf) || TYPES.count(buf))
        return {TokenType::Keyword, buf, line, startCol};
    return {TokenType::Identifier, buf, line, startCol};
}
// Функция чтения числа
Token Lexer::readNumber() {
    int startCol = column;
    std::string buf; // Буфер, в который записываем считанные символы
    // Считывание чисел
    while (!eof() && (std::isdigit(peek()) || peek() == '.'))
        buf += get();
    return {TokenType::Number, buf, line, startCol};
}
// Функция чтения оператора
Token Lexer::readOperator() {
    int startCol = column;
    std::string buf; // Буфер, в который записываем предполагаемый оператор
    // Считываем символы и проверяем, есть ли там оператор
    buf += get();
    std::string two = buf + peek();
    if (!eof()) {
        if (OPERATORS.count(two))
            buf += get();
    }
    if (!eof()) {
        std::string three = two + peek();
        if (OPERATORS.count(three))
            buf += get();
    }
    // Если не определили оператор, то выводим ошибку
    if (!OPERATORS.count(buf))
        throw std::runtime_error(
            "Неизвестный символ (" + std::to_string(line) + ":" +
            std::to_string(startCol) + ")");
    return {TokenType::Operator, buf, line, startCol};
}
// Функция чтения разделителя
Token Lexer::readSeparator() {
    char c = get();
    return {TokenType::Separator, std::string(1, c), line, column - 1};
}
>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
