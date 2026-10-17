#include "RegAlloc.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>

namespace {
bool isNumber(const std::string& value) {
    return !value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isdigit(c);
    });
}
}

Allocation RegisterAllocator::allocate(const std::vector<TACInstruction>& instructions) const {
    Allocation result;
    std::map<std::string, LiveRange> ranges;

    auto touch = [&ranges](const std::string& name, std::size_t index) {
        auto found = ranges.find(name);
        if (found == ranges.end()) ranges.emplace(name, LiveRange{name, index, index});
        else found->second.last = index;
    };

    // In straight-line TAC, a value is live from its first appearance to its last use.
    for (std::size_t i = 0; i < instructions.size(); ++i) {
        const auto& instruction = instructions[i];
        touch(instruction.destination, i);
        if (!isNumber(instruction.left)) touch(instruction.left, i);
        if (!instruction.right.empty() && !isNumber(instruction.right)) touch(instruction.right, i);
    }

    for (auto& entry : ranges) result.ranges.push_back(entry.second);
    std::sort(result.ranges.begin(), result.ranges.end(), [](const auto& a, const auto& b) {
        if (a.first != b.first) return a.first < b.first;
        return a.last < b.last;
    });

    struct ActiveValue { LiveRange range; int reg; };
    std::vector<ActiveValue> active;
    std::set<int> freeRegisters{0, 1, 2, 3};

    auto spill = [&result](const std::string& name) {
        result.location[name] = "[spill" + std::to_string(result.spillCount++) + "]";
    };

    for (const auto& range : result.ranges) {
        std::sort(active.begin(), active.end(), [](const auto& a, const auto& b) {
            return a.range.last < b.range.last;
        });
        while (!active.empty() && active.front().range.last < range.first) {
            freeRegisters.insert(active.front().reg);
            active.erase(active.begin());
        }

        if (!freeRegisters.empty()) {
            const int reg = *freeRegisters.begin();
            freeRegisters.erase(freeRegisters.begin());
            result.location[range.name] = "R" + std::to_string(reg);
            active.push_back({range, reg});
        } else if (!active.empty() && active.back().range.last > range.last) {
            const auto replaced = active.back();
            result.location[range.name] = "R" + std::to_string(replaced.reg);
            spill(replaced.range.name);
            active.pop_back();
            active.push_back({range, replaced.reg});
        } else {
            spill(range.name);
        }
    }
    return result;
}
