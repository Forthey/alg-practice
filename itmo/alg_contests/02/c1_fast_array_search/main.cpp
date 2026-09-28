#include <algorithm>
#include <cstdint>
#include <generator>
#include <iostream>
#include <ranges>
#include <vector>

using Value = std::int64_t;

std::size_t countInRange(const std::vector<Value>& array, Value min, Value max) {
    const auto minLower = std::ranges::lower_bound(array, min);
    const auto maxUpper = std::ranges::upper_bound(array, max);

    if (minLower == array.end() || maxUpper == array.begin() || maxUpper == minLower) {
        return 0;
    }

    if (maxUpper == array.end()) {
        return array.size() - (minLower - array.begin());
    }

    return maxUpper - minLower;
}

std::vector<Value> input() {
    std::uint32_t n;

    std::cin >> n;

    std::vector<Value> result;
    result.reserve(n);

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        Value value;
        std::cin >> value;

        result.emplace_back(value);
    }

    return result;
}

std::generator<std::pair<Value, Value>> requests() {
    std::uint32_t k;
    std::cin >> k;

    for ([[maybe_unused]] auto i : std::views::iota(0u, k)) {
        Value min;
        Value max;
        std::cin >> min >> max;

        co_yield {min, max};
    }
}

int main() {
    auto array = input();

    std::ranges::sort(array);

    for (const auto& [min, max] : requests()) {
        std::cout << countInRange(array, min, max) << std::endl;
    }

    return 0;
}
