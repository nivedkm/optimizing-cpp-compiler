#include "Optimizer.hpp"

#include <limits>

std::unique_ptr<Expr> Optimizer::fold(std::unique_ptr<Expr> expression) {
    auto* binary = dynamic_cast<BinaryExpr*>(expression.get());
    if (!binary) return expression;

    binary->left = fold(std::move(binary->left));
    binary->right = fold(std::move(binary->right));
    auto* left = dynamic_cast<NumberExpr*>(binary->left.get());
    auto* right = dynamic_cast<NumberExpr*>(binary->right.get());
    if (!left || !right || (binary->op == '/' && right->value == 0)) return expression;

    long long result;
    switch (binary->op) {
    case '+': result = static_cast<long long>(left->value) + right->value; break;
    case '-': result = static_cast<long long>(left->value) - right->value; break;
    case '*': result = static_cast<long long>(left->value) * right->value; break;
    case '/': result = static_cast<long long>(left->value) / right->value; break;
    default: return expression;
    }

    if (result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max())
        return expression;
    return std::make_unique<NumberExpr>(static_cast<int>(result));
}

void Optimizer::optimize(Program& program) {
    for (auto& statement : program.statements) {
        if (auto* declaration = dynamic_cast<LetStatement*>(statement.get()))
            declaration->value = fold(std::move(declaration->value));
        else if (auto* assignment = dynamic_cast<AssignStatement*>(statement.get()))
            assignment->value = fold(std::move(assignment->value));
    }
}
