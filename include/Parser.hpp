#pragma once

#include "AST.hpp"
#include "Token.hpp"

#include <vector>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}
    Program parse();

private:
    const std::vector<Token>& tokens_;
    std::size_t position_ = 0;

    const Token& peek() const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const char* message);
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> assignment();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> primary();
};
