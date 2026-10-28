#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

class RandomGenerator {
public:
    explicit RandomGenerator(std::uint32_t a, std::uint32_t b) : m_a{a}, m_b{b} {}

    std::uint32_t nextRand32() {
        std::uint32_t x = nextRand24(), y = nextRand24();
        return (x << 8) ^ y;  // число от 0 до 2^{32}-1
    }

private:
    std::uint32_t nextRand24() {
        m_cur = m_cur * m_a + m_b;  // вычисляется с переполнениями
        return m_cur >> 8;          // число от 0 до 2^{24}-1
    }

    std::uint32_t m_a;
    std::uint32_t m_b;
    std::uint32_t m_cur{};
};

using Coord = std::int64_t;
using Coords = std::vector<Coord>;
using CoordsView = std::span<Coord>;

Coord findMinimalDistSum(CoordsView coords) {
    if (coords.empty()) {
        return {};
    }

    const auto middle = coords.begin() + static_cast<long long>(coords.size() - 1) / 2;

    std::ranges::nth_element(coords, middle);

    return std::ranges::fold_left(coords, 0.0, [&middle](Coord acc, Coord x) { return acc + std::abs(*middle - x); });
}

Coords coords() {
    std::size_t n;
    std::uint32_t a;
    std::uint32_t b;

    std::cin >> n >> a >> b;

    Coords result(n);
    RandomGenerator rng{a, b};

    for (auto i : std::views::iota(0u, n)) {
        result[i] = rng.nextRand32();
    }

    return result;
}

int main() {
    auto array = coords();

    std::cout << findMinimalDistSum(array) << '\n';

    return 0;
}
