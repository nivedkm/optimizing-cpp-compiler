#include "IR.hpp"
#include "Lexer.hpp"
#include "Optimizer.hpp"
#include "Parser.hpp"
#include "RegAlloc.hpp"
#include "CodeGen.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    bool showTokens = false;
    bool showIR = false;
    bool showAllocation = false;
    std::string inputPath;
    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "--help" || argument == "-h") {
            std::cout << "Usage: " << argv[0] << " [--tokens | --ir] [--alloc] [source-file]\n"
                      << "Reads source from a file or stdin and prints allocated assembly.\n";
            return 0;
        }
        if (argument == "--tokens") {
            showTokens = true;
        } else if (argument == "--ir") {
            showIR = true;
        } else if (argument == "--alloc") {
            showAllocation = true;
        } else if (!argument.empty() && argument[0] == '-') {
            std::cerr << "unknown option: " << argument << '\n';
            return 1;
        } else if (inputPath.empty()) {
            inputPath = argument;
        } else {
            std::cerr << "only one input file may be specified\n";
            return 1;
        }
    }
    if (showTokens && (showIR || showAllocation)) {
        std::cerr << "--tokens cannot be combined with --ir or --alloc\n";
        return 1;
    }

    try {
        std::string source;
        if (inputPath.empty()) {
            source.assign(std::istreambuf_iterator<char>(std::cin), {});
        } else {
            std::ifstream input(inputPath);
            if (!input) throw std::runtime_error("cannot open input file: " + inputPath);
            source.assign(std::istreambuf_iterator<char>(input), {});
        }

        Lexer lexer(source);
        const auto tokens = lexer.tokenize();
        if (showTokens) {
            for (const auto& token : tokens)
                std::cout << token.line << ':' << token.column << ' '
                          << tokenTypeName(token.type) << " '" << token.text << "'\n";
            return 0;
        }

        Parser parser(tokens);
        auto program = parser.parse();
        Optimizer optimizer;
        optimizer.optimize(program);
        TACGenerator tac;
        const auto instructions = tac.generate(program);
        RegisterAllocator allocator;
        const auto allocation = allocator.allocate(instructions);
        if (showAllocation) {
            for (const auto& range : allocation.ranges)
                std::cerr << range.name << " [" << range.first << ", " << range.last
                          << "] -> " << allocation.location.at(range.name) << '\n';
            std::cerr << "spill slots: " << allocation.spillCount << '\n';
        }
        if (showIR) {
            for (const auto& instruction : instructions)
                std::cout << formatTAC(instruction) << '\n';
            return 0;
        }

        AssemblyEmitter emitter;
        for (const auto& line : emitter.emit(instructions, allocation))
            std::cout << line << '\n';
    } catch (const std::exception& error) {
        std::cerr << "compiler: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
