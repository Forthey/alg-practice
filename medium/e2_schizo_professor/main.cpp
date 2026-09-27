#include <cstdint>
#include <deque>
#include <iostream>
#include <ranges>

void calcDeskResult(std::deque<std::int64_t>& desk, std::uint32_t iterCount) {
    if (desk.empty()) {
        return;
    }

    // Оптимизация :>
    if (desk.size() == 1) {
        desk.front() = 0;
        return;
    }

    constexpr std::int64_t mod = std::int64_t{1} << 30;

    for ([[maybe_unused]] auto i : std::views::iota(0u, iterCount)) {
        if (desk.front() < desk.back()) {
            desk.push_back((desk.front() + desk.back()) % mod);
            desk.pop_front();
            continue;
        }

        desk.push_front((desk.back() - desk.front() + mod) % mod);
        desk.pop_back();
    }
 }

std::pair<std::deque<std::int64_t>, std::uint32_t> readBaseNumbers() {
    std::uint16_t n;
    std::uint32_t k;

    std::cin >> n >> k;

    std::deque<std::int64_t> numbers;

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        std::int64_t number;

        std::cin >> number;

        numbers.emplace_back(number);
    }

    return {numbers, k};
}

int main() {
    auto [desk, iterCount] = readBaseNumbers();

    calcDeskResult(desk, iterCount);

    while (!desk.empty()) {
        std::cout << desk.front() << " ";
        desk.pop_front();
    }
    std::cout << std::endl;

    return 0;
}
