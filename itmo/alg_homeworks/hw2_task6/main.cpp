#include <deque>
#include <limits>
#include <ranges>

#include "solution.h"

std::vector<int> findMaxSubSegment(const std::vector<int>& nums, std::size_t l, std::size_t r) {
    std::vector<int> prefSums;
    prefSums.reserve(nums.size() + 1);

    prefSums.push_back(0);

    for (const auto& num : nums) {
        prefSums.push_back(prefSums.back() + num);
    }

    int maxSum = std::numeric_limits<int>::min();
    std::size_t left{};
    std::size_t right{};

    std::deque<std::size_t> prefSumsIdx;

    for (const auto j : std::views::iota(l, prefSums.size())) {
        const auto windowLeft = j < r ? 0 : j - r;
        const auto windowRight = j - l;

        while (!prefSumsIdx.empty() && prefSums[prefSumsIdx.back()] > prefSums[windowRight]) {
            prefSumsIdx.pop_back();
        }

        prefSumsIdx.push_back(windowRight);

        if (prefSumsIdx.empty()) {
            continue;
        }

        if (prefSumsIdx.front() < windowLeft) {
            prefSumsIdx.pop_front();
        }

        if (maxSum < prefSums[j] - prefSums[prefSumsIdx.front()]) {
            maxSum = prefSums[j] - prefSums[prefSumsIdx.front()];
            left = prefSumsIdx.front();
            right = j;
        }
    }

    return {nums.begin() + left, nums.begin() + right};
}
