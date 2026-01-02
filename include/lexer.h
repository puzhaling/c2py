/**
 * \file
 * \brief C Language Lexical Analyzer
 * 
 * \section overview Overview
 * This file contains the declaration of the Lexer class which performs lexical analysis
 * of C language source code. The lexer converts a stream of source code characters
 * into a sequence of tokens for subsequent syntactic analysis.
 * 
 * \section usage Usage Example
 * \code
 * Lexer lexer("int main() { return 0; }");
 * auto tokens = lexer.tokenize();
 * for (const auto& token : tokens) {
 *     std::cout << token.lexeme << token.type << std::endl;
 * }
 * \endcode
*/

#pragma once
#include "token.h"
#include "tables.h"
#include <vector>

/**
 * \brief Performs lexical analysis of C source code
 * 
 * The Lexer class scans C language source code character by character and
 * groups them into meaningful tokens according to C language grammar rules.
 * It handles the lowest level of compilation - converting raw text into
 * structured tokens.
 * 
 * \warning The lexer does not perform semantic validation, only lexical analysis.
 * 
 * \see Token
 */
class Lexer {
public:
    /**
     * \param src C language source code
    */
    explicit Lexer(const std::string& src);

    /**
     * \brief Converts the source code string into a set of tokens
     * \returns These tokens
    */
    std::vector<Token> tokenize();
private:
    /**
     * \brief Takes a look at the current char in the source code
     * \returns That char
    */
    char peek() const;

    /**
     * \brief Takes a look at the current char in the source code
     * and go to the next one
     * \returns That char
    */
    char get();

    /**
     * \brief Is the source code finished or not? That is the question
     * \returns The answer
    */
    bool eof() const;

    /**
     * \brief Moves to the first non-whitespace character
    */
    void skipWhitespace();

    /**
     * \brief Reads an identifier or keyword from the source
     * code and returns it as a token
     * \returns That token
    */
    Token readIdentifierOrKeyword();

    /**
     * \brief Reads a number from the source
     * code and returns it as a token
     * \returns That token
    */
    Token readNumber();

    /**
     * \brief Reads an operator from the source
     * code and returns it as a token
     * \returns That token
    */
    Token readOperator();

    /**
     * \brief Reads a separator from the source
     * code and returns it as a token
     * \returns That token
    */
    Token readSeparator(); // Чтение разделителя

    /**
     * \name Lexer Internal State
     * Lexer's internal state
    */
    /**@{*/
    const std::string source; /**< Saved C language source code */
    size_t pos; /**< Position in the current line */
    int line; /**< Current line number */
    /**
    * Current column number
    * \invariant Equals to \link pos \endlink
    */
    int column;
    /**@}*/
};
