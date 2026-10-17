#pragma once

#include "AST.hpp"

class Optimizer {
public:
    void optimize(Program& program);

private:
    std::unique_ptr<Expr> fold(std::unique_ptr<Expr> expression);
};
