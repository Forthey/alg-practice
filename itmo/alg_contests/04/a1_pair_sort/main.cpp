#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

using Value = std::uint32_t;
using Pair = std::pair<Value, Value>;
using Element = std::pair<Pair, std::size_t>;

std::vector<Element> input() {
    std::size_t n;

    std::cin >> n;

    std::vector<Element> result(n);

    for (auto i : std::views::iota(0u, n)) {
        Pair pair;

        std::cin >> pair.first >> pair.second;

        result[i] = {pair, i + 1};
    }

    return result;
}

int main() {
    auto array = input();

    std::ranges::sort(array);

    for (const auto& index : array | std::views::values) {
        std::cout << index << " ";
    }

    return 0;
}
