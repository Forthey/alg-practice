#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

using Value = std::int64_t;
using Array = std::vector<Value>;
using Depth = std::size_t;

Depth sort(std::span<Value> arr, Depth depth = 0) {
    if (arr.size() <= 1) {
        return depth;
    }

    const auto pivot = arr[(arr.size() - 1) / 2];
    Array a;
    Array c;

    for (const auto value : arr) {
        if (value < pivot) {
            a.push_back(value);
        } else if (value > pivot) {
            c.push_back(value);
        }
    }

    return std::max(sort(std::span{a}, depth + 1), sort(std::span{c}, depth + 1));
}

Array array() {
    std::size_t n;

    std::cin >> n;

    Array result(n);

    for (auto i : std::views::iota(0u, n)) {
        std::cin >> result[i];
    }

    return result;
}

int main() {
    auto arr = array();

    std::cout << sort(std::span{arr.begin(), arr.end()}) << '\n';

    return 0;
}
