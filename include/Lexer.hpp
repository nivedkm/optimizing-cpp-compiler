#pragma once

#include "Token.hpp"

#include <string>
#include <utility>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source) : source_(std::move(source)) {}
    std::vector<Token> tokenize();

private:
    std::string source_;
    std::size_t position_ = 0;
    int line_ = 1;
    int column_ = 1;

    char peek() const;
    char advance();
    void skipWhitespace();
};
