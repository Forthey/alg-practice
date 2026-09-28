#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>
#include <vector>

using Value = std::uint32_t;

bool containsValue(const std::vector<Value>& array, Value value) {
    std::pair<std::int64_t, std::int64_t> result{-1, array.size()};

    while (result.second - result.first > 1) {
        const auto center = result.first + (result.second - result.first) / 2;

        if (array[center] < value) {
            result.first = center;
            continue;
        }

        result.second = center;
    }

    return array[result.second] == value;
}

struct Input {
    std::vector<Value> array;
    std::vector<Value> requests;
};

Input input() {
    std::uint32_t n;
    std::uint32_t k;

    std::cin >> n >> k;

    Input result;
    result.array.reserve(n);

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        Value value;
        std::cin >> value;

        result.array.emplace_back(value);
    }

    result.requests.reserve(n);

    for ([[maybe_unused]] auto i : std::views::iota(0u, k)) {
        Value value;
        std::cin >> value;

        result.requests.emplace_back(value);
    }

    return result;
}

int main() {
    const auto [array, requests] = input();

    for (const auto& request : requests) {
        std::cout << (containsValue(array, request) ? "YES" : "NO") << std::endl;
    }

    return 0;
}
