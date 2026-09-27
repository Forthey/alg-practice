#pragma once

#include <cstddef>
#include <vector>

struct Result {
    std::size_t l1;
    std::size_t l2;
    std::size_t r1;
    std::size_t r2;
};

// Two nonempty segments separated by at least one unused element.
Result findMaxPair(const std::vector<int>& a);
