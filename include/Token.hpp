#pragma once

#include <cstddef>
#include <string>
#include <utility>

enum class TokenType {
    LET,
    IDENT,
    NUMBER,
    IF,
    ELSE,

    PLUS,
    MINUS,
    STAR,
    SLASH,
    ASSIGN,
    EQ,
    NE,
    LT,
    LE,
    GT,
    GE,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::size_t line;
    std::size_t column;

    Token(TokenType tokenType, std::string tokenLexeme,
          std::size_t tokenLine, std::size_t tokenColumn)
        : type(tokenType), lexeme(std::move(tokenLexeme)),
          line(tokenLine), column(tokenColumn) {}
};

const char* tokenTypeName(TokenType type) noexcept;
