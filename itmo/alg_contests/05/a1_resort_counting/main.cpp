#include <generator>
#include <iostream>

using Number = std::uint16_t;
using NumberCount = std::size_t;

constexpr auto kNumberMax = 100;

using NumbersCountArray = std::array<NumberCount, kNumberMax + 1>;

NumberCount insertNumber(NumbersCountArray& numbersToCounts, const Number number) {
    NumberCount pos = 0;

    for (Number numIter = 0; numIter <= number; ++numIter) {
        pos += numbersToCounts[numIter];
    }

    ++numbersToCounts[number];

    return pos;
}

std::generator<Number> requests() {
    std::size_t n;

    std::cin >> n;

    for ([[maybe_unused]] auto _ : std::views::iota(0u, n)) {
        Number number;

        std::cin >> number;

        co_yield number;
    }
}

int main() {
    NumbersCountArray array{};

    for (const auto& number : requests()) {
        std::cout << insertNumber(array, number) << " ";
    }
    return 0;
}
