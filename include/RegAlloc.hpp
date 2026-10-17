#pragma once

#include "IR.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

struct LiveRange {
    std::string name;
    std::size_t first;
    std::size_t last;
};

struct Allocation {
    std::vector<LiveRange> ranges;
    std::unordered_map<std::string, std::string> location;
    std::size_t spillCount = 0;
};

class RegisterAllocator {
public:
    Allocation allocate(const std::vector<TACInstruction>& instructions) const;
};
