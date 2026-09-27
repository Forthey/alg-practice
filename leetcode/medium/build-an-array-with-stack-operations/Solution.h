#pragma once
#include <string>
#include <vector>

namespace solution {
static constexpr auto kPush = "Push";
static constexpr auto kPop = "Pop";

class Solution {
public:
    std::vector<std::string> buildArray(std::vector<int>& target, int n);
};
}  // namespace solution
