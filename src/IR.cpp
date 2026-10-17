#include "IR.hpp"

#include <stdexcept>

std::string formatTAC(const TACInstruction& instruction) {
    if (instruction.operation.empty())
        return instruction.destination + " = " + instruction.left;
    return instruction.destination + " = " + instruction.left + " " +
           instruction.operation + " " + instruction.right;
}

std::string TACGenerator::expression(const Expr& node) {
    if (const auto* number = dynamic_cast<const NumberExpr*>(&node))
        return std::to_string(number->value);
    if (const auto* variable = dynamic_cast<const VariableExpr*>(&node))
        return variable->name;
    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&node)) {
        const auto left = expression(*binary->left);
        const auto right = expression(*binary->right);
        const auto temporary = "%t" + std::to_string(nextTemporary_++);
        instructions_.push_back({temporary, std::string(1, binary->op), left, right});
        return temporary;
    }
    throw std::runtime_error("unknown expression in TAC generator");
}

std::vector<TACInstruction> TACGenerator::generate(const Program& program) {
    instructions_.clear();
    nextTemporary_ = 0;
    for (const auto& statement : program.statements) {
        if (const auto* declaration = dynamic_cast<const LetStatement*>(statement.get()))
            instructions_.push_back({declaration->name, "", expression(*declaration->value), ""});
        else if (const auto* assignment = dynamic_cast<const AssignStatement*>(statement.get()))
            instructions_.push_back({assignment->name, "", expression(*assignment->value), ""});
    }
    return instructions_;
}
