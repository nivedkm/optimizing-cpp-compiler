#pragma once

#include <string>

enum class TokenType {
    Let, Identifier, Number,
    Plus, Minus, Star, Slash, Equal,
    LeftParen, RightParen, Semicolon, End
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

const char* tokenTypeName(TokenType type);
