#include "CodeGen.hpp"

#include <cctype>
#include <stdexcept>

namespace {
bool isNumber(const std::string& value) {
    for (const unsigned char c : value)
        if (!std::isdigit(c)) return false;
    return !value.empty();
}

std::string place(const std::string& value, const Allocation& allocation) {
    if (isNumber(value)) return "#" + value;
    const auto found = allocation.location.find(value);
    if (found == allocation.location.end())
        throw std::runtime_error("no register assigned to '" + value + "'");
    return found->second;
}

const char* opcode(const std::string& operation) {
    if (operation == "+") return "ADD";
    if (operation == "-") return "SUB";
    if (operation == "*") return "MUL";
    if (operation == "/") return "DIV";
    throw std::runtime_error("unknown TAC operation: " + operation);
}
}

std::vector<std::string> AssemblyEmitter::emit(
    const std::vector<TACInstruction>& instructions, const Allocation& allocation) const {
    std::vector<std::string> output;
    for (const auto& instruction : instructions) {
        const auto destination = place(instruction.destination, allocation);
        if (instruction.operation.empty()) {
            const auto source = place(instruction.left, allocation);
            if (isNumber(instruction.left)) output.push_back("LOADI " + destination + ", " + source);
            else if (destination != source) output.push_back("MOV " + destination + ", " + source);
        } else {
            output.push_back(std::string(opcode(instruction.operation)) + " " + destination +
                ", " + place(instruction.left, allocation) + ", " +
                place(instruction.right, allocation));
        }
    }
    return output;
}
