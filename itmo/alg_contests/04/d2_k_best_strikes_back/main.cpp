#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <span>
#include <vector>

using Weight = std::int64_t;
using Value = std::int64_t;
using Metric = long double;
using Count = std::int64_t;

struct Jewel {
    Weight weight;
    Value value;
    std::size_t number;
};

using Jewels = std::vector<Jewel>;
using JewelsView = std::span<Jewel>;

bool canReachMetric(JewelsView jewels, Metric metric, Count jewelsCount) {
    const auto jewelsToDiff = [metric](const Jewel& jewel) { return jewel.value - metric * jewel.weight; };
    std::ranges::nth_element(jewels, jewels.begin() + jewelsCount, std::greater<Metric>{}, jewelsToDiff);

    return std::accumulate(jewels.begin(), jewels.begin() + jewelsCount, Metric{},
                           [&jewelsToDiff](Metric sum, const Jewel& jewel) { return sum + jewelsToDiff(jewel); }) >= 0;
}

JewelsView maximizeMetric(JewelsView jewels, Count jewelsCount) {
    if (static_cast<std::size_t>(jewelsCount) == jewels.size()) {
        return jewels;
    }

    Metric left = 0.0;
    Metric right = 1e7L / jewelsCount;  // Это 10^7 / k - максимальное в целом значение с учетом вводных

    // 50 тут получено путем шизовычислений с требованием гарантированно иметь 2 знака после запятой корректными
    for (auto i = 0; i < 50; ++i) {
        const auto middle = left + (right - left) / 2;

        if (canReachMetric(jewels, middle, jewelsCount)) {
            left = middle;
            continue;
        }

        right = middle;
    }

    canReachMetric(jewels, left, jewelsCount);
    return jewels.first(static_cast<std::size_t>(jewelsCount));
}

Jewels jewels(Count& jewelsCount) {
    std::size_t n;

    std::cin >> n >> jewelsCount;

    Jewels jewels(n);

    for (auto i : std::views::iota(0u, n)) {
        std::cin >> jewels[i].value >> jewels[i].weight;
        jewels[i].number = i + 1;
    }

    return jewels;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Count jewelsCount;
    auto array = jewels(jewelsCount);

    for (const auto& jewel : maximizeMetric(array, jewelsCount)) {
        std::cout << jewel.number << ' ';
    }
    std::cout << std::endl;

    return 0;
}
