#pragma once

#include "Token.hpp"

#include <string>
#include <vector>

class Lexer {
public: 
    explicit Lexer(std::string source);

    // Tokenizes the complete input and appends one END_OF_FILE token.
    std::vector<Token> tokenize();

private:
    std::string source_;
    std::size_t offset_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;

    bool atEnd() const noexcept;
    char peek(std::size_t lookahead = 0) const noexcept;
    char advance() noexcept;
    bool match(char expected) noexcept;
    void skipTrivia();
    Token scanToken();
    Token identifierOrKeyword();
    Token number();
    Token makeToken(TokenType type, std::size_t start,
                    std::size_t tokenLine, std::size_t tokenColumn) const;
    [[noreturn]] void error(const std::string& message,
                            std::size_t line, std::size_t column) const;
};
