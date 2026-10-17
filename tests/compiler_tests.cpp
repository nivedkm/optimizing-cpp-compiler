#include "CodeGen.hpp"
#include "Lexer.hpp"
#include "Optimizer.hpp"
#include "Parser.hpp"
#include "RegAlloc.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

std::vector<TACInstruction> compileToTAC(const std::string& source) {
    Lexer lexer(source);
    const auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    Optimizer optimizer;
    optimizer.optimize(program);
    TACGenerator generator;
    return generator.generate(program);
}

std::vector<std::string> format(const std::vector<TACInstruction>& instructions) {
    std::vector<std::string> result;
    for (const auto& instruction : instructions) result.push_back(formatTAC(instruction));
    return result;
}

void testTokensAndLocations() {
    Lexer lexer("let x = 12;\n x = x + 1;");
    const auto tokens = lexer.tokenize();
    require(tokens.front().type == TokenType::Let, "lexer should recognize let");
    require(tokens[1].type == TokenType::Identifier && tokens[1].text == "x",
            "lexer should recognize identifiers");
    require(tokens.back().type == TokenType::End, "lexer should append end token");
    require(tokens[5].line == 2, "lexer should track lines");
}

void testConstantFoldingAndPrecedence() {
    const auto tac = format(compileToTAC("let answer = 4 + 2 * 5;"));
    require(tac == std::vector<std::string>{"answer = 14"},
            "constant folding should respect multiplication precedence");

    const auto parens = format(compileToTAC("let value = (1 + 2) * 3;"));
    require(parens == std::vector<std::string>{"value = 9"},
            "parentheses should override precedence");

    const auto leftAssociative = format(compileToTAC("let value = 20 - 5 - 3;"));
    require(leftAssociative == std::vector<std::string>{"value = 12"},
            "subtraction should associate from the left");

    const auto division = format(compileToTAC("let value = 7 / 2;"));
    require(division == std::vector<std::string>{"value = 3"},
            "integer division should fold using truncation");
}

void testVariablesAndTemporaryNames() {
    const auto tac = format(compileToTAC("let t0 = 2; let result = t0 + 3;"));
    require(tac == std::vector<std::string>{"t0 = 2", "%t0 = t0 + 3", "result = %t0"},
            "generated temporaries must not collide with user identifiers");
}

void testDivisionByZeroIsNotFolded() {
    const auto tac = format(compileToTAC("let value = 7 / 0;"));
    require(tac == std::vector<std::string>{"%t0 = 7 / 0", "value = %t0"},
            "division by zero should remain an operation instead of being folded");
}

void testRegisterAllocationAndEmission() {
    const auto tac = compileToTAC("let answer = 14; let next = answer + 1;");
    RegisterAllocator allocator;
    const auto allocation = allocator.allocate(tac);
    require(allocation.spillCount == 0, "small example should fit in registers");
    AssemblyEmitter emitter;
    const auto assembly = emitter.emit(tac, allocation);
    require(assembly == std::vector<std::string>{
                "LOADI R0, #14", "ADD R1, R0, #1", "MOV R0, R1"},
            "emitter should use assigned registers and immediates");
}

void testSpilling() {
    const auto tac = compileToTAC(
        "let a = 1; let b = 2; let c = 3; let d = 4; let e = 5; "
        "let result = a + b + c + d + e;");
    RegisterAllocator allocator;
    const auto allocation = allocator.allocate(tac);
    require(allocation.spillCount > 0, "live values beyond four registers should spill");
    for (std::size_t i = 0; i < allocation.ranges.size(); ++i) {
        const auto& first = allocation.ranges[i];
        const auto& firstLocation = allocation.location.at(first.name);
        for (std::size_t j = i + 1; j < allocation.ranges.size(); ++j) {
            const auto& second = allocation.ranges[j];
            const auto& secondLocation = allocation.location.at(second.name);
            const bool overlap = first.first <= second.last && second.first <= first.last;
            if (overlap && firstLocation.rfind("R", 0) == 0 &&
                secondLocation.rfind("R", 0) == 0)
                require(firstLocation != secondLocation,
                        "overlapping live ranges cannot share a register");
        }
    }
    AssemblyEmitter emitter;
    const auto assembly = emitter.emit(tac, allocation);
    bool mentionsSpill = false;
    for (const auto& line : assembly)
        if (line.find("[spill") != std::string::npos) mentionsSpill = true;
    require(mentionsSpill, "emitted instructions should use allocated spill locations");
}

void testInvalidSyntax() {
    bool failed = false;
    try {
        (void)compileToTAC("let x = 1 + ;");
    } catch (const std::runtime_error&) {
        failed = true;
    }
    require(failed, "invalid syntax should be reported");
}
} // namespace

int main() {
    try {
        testTokensAndLocations();
        testConstantFoldingAndPrecedence();
        testVariablesAndTemporaryNames();
        testDivisionByZeroIsNotFolded();
        testRegisterAllocationAndEmission();
        testSpilling();
        testInvalidSyntax();
    } catch (const std::exception& error) {
        std::cerr << "test failure: " << error.what() << '\n';
        return 1;
    }
    std::cout << "all compiler tests passed\n";
    return 0;
}
