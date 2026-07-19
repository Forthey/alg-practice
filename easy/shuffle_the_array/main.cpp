#include <iostream>
#include <ostream>
#include <vector>

#include "StdOutput.h"

template <typename T>
std::vector<T> shuffleImpl(const std::vector<T>& nums) {
    const auto partSize = nums.size() / 2;
    std::vector<T> shuffled(nums.size());

    for (std::size_t i = 0; i < shuffled.size(); ++i) {
        if (i % 2 == 0) {
            shuffled[i] = nums[i / 2];
        } else {
            shuffled[i] = nums[partSize + i / 2];
        }
    }

    return shuffled;
}

int main() {
    std::cout << shuffleImpl<int>({2,5,1,3,4,7}) << std::endl;
    std::cout << shuffleImpl<int>({1,2,3,4,4,3,2,1}) << std::endl;
    std::cout << shuffleImpl<int>({1,1,2,2}) << std::endl;
    return 0;
}
