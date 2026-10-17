#include "Lexer.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

const char* tokenTypeName(TokenType type) {
    switch (type) {
    case TokenType::Let: return "LET";
    case TokenType::Identifier: return "IDENTIFIER";
    case TokenType::Number: return "NUMBER";
    case TokenType::Plus: return "PLUS";
    case TokenType::Minus: return "MINUS";
    case TokenType::Star: return "STAR";
    case TokenType::Slash: return "SLASH";
    case TokenType::Equal: return "EQUAL";
    case TokenType::LeftParen: return "LEFT_PAREN";
    case TokenType::RightParen: return "RIGHT_PAREN";
    case TokenType::Semicolon: return "SEMICOLON";
    case TokenType::End: return "END";
    }
    return "UNKNOWN";
}

char Lexer::peek() const {
    return position_ < source_.size() ? source_[position_] : '\0';
}

char Lexer::advance() {
    const char c = source_[position_++];
    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(static_cast<unsigned char>(peek()))) advance();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (position_ < source_.size()) {
        skipWhitespace();
        if (position_ == source_.size()) break;

        const int startLine = line_;
        const int startColumn = column_;
        const char c = peek();
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            const auto start = position_;
            while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') advance();
            const auto text = source_.substr(start, position_ - start);
            tokens.push_back({text == "let" ? TokenType::Let : TokenType::Identifier,
                              text, startLine, startColumn});
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            const auto start = position_;
            while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
            tokens.push_back({TokenType::Number, source_.substr(start, position_ - start),
                              startLine, startColumn});
        } else {
            advance();
            TokenType type;
            switch (c) {
            case '+': type = TokenType::Plus; break;
            case '-': type = TokenType::Minus; break;
            case '*': type = TokenType::Star; break;
            case '/': type = TokenType::Slash; break;
            case '=': type = TokenType::Equal; break;
            case '(': type = TokenType::LeftParen; break;
            case ')': type = TokenType::RightParen; break;
            case ';': type = TokenType::Semicolon; break;
            default:
                throw std::runtime_error("unexpected character at " +
                    std::to_string(startLine) + ":" + std::to_string(startColumn));
            }
            tokens.push_back({type, std::string(1, c), startLine, startColumn});
        }
    }
    tokens.push_back({TokenType::End, "", line_, column_});
    return tokens;
}
