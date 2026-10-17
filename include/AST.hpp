#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr final : Expr {
    explicit NumberExpr(int value) : value(value) {}
    int value;
};

struct VariableExpr final : Expr {
    explicit VariableExpr(std::string name) : name(std::move(name)) {}
    std::string name;
};

struct BinaryExpr final : Expr {
    BinaryExpr(char op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    char op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Statement {
    virtual ~Statement() = default;
};

struct LetStatement final : Statement {
    LetStatement(std::string name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}
    std::string name;
    std::unique_ptr<Expr> value;
};

struct AssignStatement final : Statement {
    AssignStatement(std::string name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}
    std::string name;
    std::unique_ptr<Expr> value;
};

struct Program {
    std::vector<std::unique_ptr<Statement>> statements;
};
