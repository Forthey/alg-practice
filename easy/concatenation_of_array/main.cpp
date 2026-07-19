#include <iostream>
#include <ostream>
#include <vector>

#include "StdOutput.h"

std::vector<int> getConcatenationImpl(const std::vector<int>& nums) {
    std::vector<int> result(2 * nums.size());

    for (auto i = 0ull; i < result.size(); ++i) {
        result[i] = nums[i % nums.size()];
    }

    return result;
}

int main() {
    std::cout << getConcatenationImpl({1,2,3,4,5}) << std::endl;
    return 0;
}
