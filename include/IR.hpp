#pragma once

#include "AST.hpp"

#include <string>
#include <vector>

struct TACInstruction {
    std::string destination;
    std::string operation; // Empty for a simple assignment.
    std::string left;
    std::string right;
};

std::string formatTAC(const TACInstruction& instruction);

class TACGenerator {
public:
    std::vector<TACInstruction> generate(const Program& program);

private:
    int nextTemporary_ = 0;
    std::vector<TACInstruction> instructions_;

    std::string expression(const Expr& expression);
};
