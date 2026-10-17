#pragma once

#include "IR.hpp"
#include "RegAlloc.hpp"

#include <string>
#include <vector>

class AssemblyEmitter {
public:
    std::vector<std::string> emit(const std::vector<TACInstruction>& instructions,
                                  const Allocation& allocation) const;
};
