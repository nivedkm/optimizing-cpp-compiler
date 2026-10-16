#include "Lexer.hpp"

#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <utility>

const char* tokenTypeName(TokenType type) noexcept {
    switch (type) {
    case TokenType::LET: return "LET";
    case TokenType::IDENT: return "IDENT";
    case TokenType::NUMBER: return "NUMBER";
    case TokenType::IF: return "IF";
    case TokenType::ELSE: return "ELSE";
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINUS: return "MINUS";
    case TokenType::STAR: return "STAR";
    case TokenType::SLASH: return "SLASH";
    case TokenType::ASSIGN: return "ASSIGN";
    case TokenType::EQ: return "EQ";
    case TokenType::NE: return "NE";
    case TokenType::LT: return "LT";
    case TokenType::LE: return "LE";
    case TokenType::GT: return "GT";
    case TokenType::GE: return "GE";
    case TokenType::LPAREN: return "LPAREN";
    case TokenType::RPAREN: return "RPAREN";
    case TokenType::LBRACE: return "LBRACE";
    case TokenType::RBRACE: return "RBRACE";
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::END_OF_FILE: return "EOF";
    }
    return "UNKNOWN";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

bool Lexer::atEnd() const noexcept { return offset_ >= source_.size(); }

char Lexer::peek(std::size_t lookahead) const noexcept {
    const auto index = offset_ + lookahead;
    return index < source_.size() ? source_[index] : '\0';
}

char Lexer::advance() noexcept {
    if (atEnd()) return '\0';
    const char c = source_[offset_++];
    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return c;
}

bool Lexer::match(char expected) noexcept {
    if (atEnd() || peek() != expected) return false;
    advance();
    return true;
}

void Lexer::skipTrivia() {
    for (;;) {
        while (std::isspace(static_cast<unsigned char>(peek()))) advance();
        if (peek() == '/' && peek(1) == '/') {
            while (!atEnd() && peek() != '\n') advance();
            continue;
        }
        if (peek() == '/' && peek(1) == '*') {
            const auto startLine = line_;
            const auto startColumn = column_;
            advance();
            advance();
            while (!(peek() == '*' && peek(1) == '/') && !atEnd()) advance();
            if (atEnd()) error("unterminated block comment", startLine, startColumn);
            advance();
            advance();
            continue;
        }
        return;
    }
}

Token Lexer::makeToken(TokenType type, std::size_t start,
                      std::size_t tokenLine, std::size_t tokenColumn) const {
    return Token(type, source_.substr(start, offset_ - start), tokenLine, tokenColumn);
}

Token Lexer::identifierOrKeyword() {
    const auto start = offset_ - 1;
    const auto tokenLine = line_;
    const auto tokenColumn = column_ - 1;
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') advance();
    auto token = makeToken(TokenType::IDENT, start, tokenLine, tokenColumn);
    static const std::unordered_map<std::string, TokenType> keywords{
        {"let", TokenType::LET}, {"if", TokenType::IF}, {"else", TokenType::ELSE}
    };
    const auto found = keywords.find(token.lexeme);
    if (found != keywords.end()) token.type = found->second;
    return token;
}

Token Lexer::number() {
    const auto start = offset_ - 1;
    const auto tokenLine = line_;
    const auto tokenColumn = column_ - 1;
    while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
    return makeToken(TokenType::NUMBER, start, tokenLine, tokenColumn);
}

Token Lexer::scanToken() {
    skipTrivia();
    const auto start = offset_;
    const auto tokenLine = line_;
    const auto tokenColumn = column_;
    if (atEnd()) return Token(TokenType::END_OF_FILE, "", line_, column_);

    const char c = advance();
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') return identifierOrKeyword();
    if (std::isdigit(static_cast<unsigned char>(c))) return number();

    switch (c) {
    case '+': return makeToken(TokenType::PLUS, start, tokenLine, tokenColumn);
    case '-': return makeToken(TokenType::MINUS, start, tokenLine, tokenColumn);
    case '*': return makeToken(TokenType::STAR, start, tokenLine, tokenColumn);
    case '/': return makeToken(TokenType::SLASH, start, tokenLine, tokenColumn);
    case '(': return makeToken(TokenType::LPAREN, start, tokenLine, tokenColumn);
    case ')': return makeToken(TokenType::RPAREN, start, tokenLine, tokenColumn);
    case '{': return makeToken(TokenType::LBRACE, start, tokenLine, tokenColumn);
    case '}': return makeToken(TokenType::RBRACE, start, tokenLine, tokenColumn);
    case ';': return makeToken(TokenType::SEMICOLON, start, tokenLine, tokenColumn);
    case '=': return makeToken(match('=') ? TokenType::EQ : TokenType::ASSIGN, start, tokenLine, tokenColumn);
    case '!':
        if (match('=')) return makeToken(TokenType::NE, start, tokenLine, tokenColumn);
        error("expected '=' after '!'", tokenLine, tokenColumn);
    case '<': return makeToken(match('=') ? TokenType::LE : TokenType::LT, start, tokenLine, tokenColumn);
    case '>': return makeToken(match('=') ? TokenType::GE : TokenType::GT, start, tokenLine, tokenColumn);
    default: error(std::string("unexpected character '") + c + "'", tokenLine, tokenColumn);
    }
}

[[noreturn]] void Lexer::error(const std::string& message,
                               std::size_t line, std::size_t column) const {
    throw std::runtime_error("Lexer error at " + std::to_string(line) + ":" +
                             std::to_string(column) + ": " + message);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    for (;;) {
        auto token = scanToken();
        const bool done = token.type == TokenType::END_OF_FILE;
        tokens.push_back(std::move(token));
        if (done) return tokens;
    }
}
