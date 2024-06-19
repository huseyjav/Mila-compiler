#include <cctype>
#include <stdexcept>

#include "Lexer.hpp"

/**
 * @brief Function to return the next token from standard input
 *
 * the variable 'm_IdentifierStr' is set there in case of an identifier,
 * the variable 'm_NumVal' is set there in case of a number.
 */
int Lexer::gettok() {
    // char curCh;
    while (isspace(is.peek()) && !is.eof()) is.get();
    if (is.eof()) return Token::tok_eof;
    m_IdentifierStr.clear();
    while (isalnum(is.peek()) || is.peek()=='$' || is.peek()=='&') m_IdentifierStr += is.get();
    // if(m_IdentifierStr.empty()) throw std::invalid_argument("");
    // std::cout << m_IdentifierStr << std::endl;
    if (m_IdentifierStr == "begin") return Token::tok_begin;
    if (m_IdentifierStr == "end") return Token::tok_end;
    if (m_IdentifierStr == "const") return Token::tok_const;
    if (m_IdentifierStr == "procedure") return Token::tok_procedure;
    if (m_IdentifierStr == "forward") return Token::tok_forward;
    if (m_IdentifierStr == "function") return Token::tok_function;
    if (m_IdentifierStr == "if") return Token::tok_if;
    if (m_IdentifierStr == "then") return Token::tok_then;
    if (m_IdentifierStr == "else") return Token::tok_else;
    if (m_IdentifierStr == "program") return Token::tok_program;
    if (m_IdentifierStr == "while") return Token::tok_while;
    if (m_IdentifierStr == "exit") return Token::tok_exit;
    if (m_IdentifierStr == "var") return Token::tok_var;
    if (m_IdentifierStr == "integer") return Token::tok_integer;
    if (m_IdentifierStr == "for") return Token::tok_for;
    if (m_IdentifierStr == "do") return Token::tok_do;
    // keywords for loop
    if (m_IdentifierStr == "to") return Token::tok_to;
    if (m_IdentifierStr == "downto") return Token::tok_downto;
    // keyword for array
    if (m_IdentifierStr == "array") return Token::tok_array;
    // keyword for 3 char operations
    if (m_IdentifierStr == "mod") return Token::tok_mod;
    if (m_IdentifierStr == "div") return Token::tok_div;
    if (m_IdentifierStr == "not") return Token::tok_not;
    if (m_IdentifierStr == "and") return Token::tok_and;
    if (m_IdentifierStr == "xor") return Token::tok_xor;
    if (m_IdentifierStr == "or") return Token::tok_or;
    if (m_IdentifierStr == "break") return Token::tok_break;

    if (convertNumber()) return Token::tok_number;

    if (!m_IdentifierStr.empty()) return Token::tok_identifier;

    if (is.peek() == '!') {
        m_IdentifierStr += is.get();
        if (is.peek() != '=') goto single_char;
        is.get();
        return Token::tok_notequal;
    }
    if (is.peek() == '<') {
        m_IdentifierStr += is.get();
        if (is.peek() != '=') goto single_char;
        m_IdentifierStr += is.get();
        return Token::tok_lessequal;
    }
    if (is.peek() == '>') {
        m_IdentifierStr += is.get();
        if (is.peek() != '=') goto single_char;
        m_IdentifierStr += is.get();
        return Token::tok_greaterequal;
    }
    if (is.peek() == ':') {
        m_IdentifierStr += is.get();
        if (is.peek() != '=') goto single_char;
        m_IdentifierStr += is.get();
        return Token::tok_assign;
    }
    if (is.peek() == '|') {
        m_IdentifierStr += is.get();
        if (is.peek() != '|') goto single_char;
        m_IdentifierStr += is.get();
        return Token::tok_or;
    }
    if (is.eof()) return Token::tok_eof;
    m_IdentifierStr += is.get();
single_char:
    return m_IdentifierStr.back();
}
