#include "Parser.hpp"

#include <stdexcept>
#include <string>

const Token& Parser::peek() const { return tokens_[position_]; }

bool Parser::match(TokenType type) {
    if (peek().type != type) return false;
    ++position_;
    return true;
}

const Token& Parser::consume(TokenType type, const char* message) {
    if (peek().type == type) return tokens_[position_++];
    throw std::runtime_error("parser error at " + std::to_string(peek().line) + ":" +
        std::to_string(peek().column) + ": " + message);
}

Program Parser::parse() {
    Program program;
    while (peek().type != TokenType::End)
        program.statements.push_back(statement());
    return program;
}

std::unique_ptr<Statement> Parser::statement() {
    if (match(TokenType::Let)) return declaration();
    return assignment();
}

std::unique_ptr<Statement> Parser::declaration() {
    const auto name = consume(TokenType::Identifier, "expected a variable name").text;
    consume(TokenType::Equal, "expected '=' after variable name");
    auto value = expression();
    consume(TokenType::Semicolon, "expected ';' after declaration");
    return std::make_unique<LetStatement>(name, std::move(value));
}

std::unique_ptr<Statement> Parser::assignment() {
    const auto name = consume(TokenType::Identifier, "expected a statement").text;
    consume(TokenType::Equal, "expected '=' after variable name");
    auto value = expression();
    consume(TokenType::Semicolon, "expected ';' after assignment");
    return std::make_unique<AssignStatement>(name, std::move(value));
}

std::unique_ptr<Expr> Parser::expression() { return term(); }

std::unique_ptr<Expr> Parser::term() {
    auto left = factor();
    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
        const char op = tokens_[position_++].text[0];
        left = std::make_unique<BinaryExpr>(op, std::move(left), factor());
    }
    return left;
}

std::unique_ptr<Expr> Parser::factor() {
    auto left = primary();
    while (peek().type == TokenType::Star || peek().type == TokenType::Slash) {
        const char op = tokens_[position_++].text[0];
        left = std::make_unique<BinaryExpr>(op, std::move(left), primary());
    }
    return left;
}

std::unique_ptr<Expr> Parser::primary() {
    if (peek().type == TokenType::Number) {
        const auto token = tokens_[position_++];
        try {
            return std::make_unique<NumberExpr>(std::stoi(token.text));
        } catch (const std::exception&) {
            throw std::runtime_error("integer literal out of range at " +
                std::to_string(token.line) + ":" + std::to_string(token.column));
        }
    }
    if (peek().type == TokenType::Identifier)
        return std::make_unique<VariableExpr>(tokens_[position_++].text);
    if (match(TokenType::LeftParen)) {
        auto value = expression();
        consume(TokenType::RightParen, "expected ')' after expression");
        return value;
    }
    consume(TokenType::Number, "expected a number, variable, or '(' expression ')'");
    return nullptr;
}
