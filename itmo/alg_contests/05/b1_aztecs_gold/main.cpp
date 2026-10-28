#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <ranges>

namespace {
using IntValue = std::uint64_t;
using Weight = IntValue;
using Cost = IntValue;

struct Item {
    Weight weight{};
    Cost cost{};
};

constexpr auto kItemsMaxCount = 25;
constexpr auto kPrecisionToInt = 1e11;

using Items = std::array<std::optional<Item>, kItemsMaxCount>;

Cost calcMaxCost(const Items& items, Weight maxWeight, std::size_t currentElement, Cost currentCost,
                 Weight currentWeight) {
    if (currentWeight >= maxWeight || currentElement >= items.size() || !items[currentElement]) {
        return currentCost;
    }

    const auto costIfNotPick = calcMaxCost(items, maxWeight, currentElement + 1, currentCost, currentWeight);
    const auto weightIfPick = currentWeight + items[currentElement]->weight;

    if (weightIfPick > maxWeight) {
        return costIfNotPick;
    }

    const auto costIfPick =
        calcMaxCost(items, maxWeight, currentElement + 1, currentCost + items[currentElement]->cost, weightIfPick);

    return std::max(costIfPick, costIfNotPick);
}

constexpr IntValue doubleToInt(double value) { return static_cast<IntValue>(value * kPrecisionToInt); }
constexpr double intToDouble(IntValue value) { return static_cast<double>(value) / kPrecisionToInt; }

std::pair<Items, Weight> getItems() {
    std::uint16_t n;
    double weight;

    std::cin >> n >> weight;

    Items items{};

    for (const auto i : std::views::iota(std::uint16_t{}, n)) {
        double itemWeight;
        double itemCost;

        std::cin >> itemWeight >> itemCost;

        items[i] = {
            .weight = doubleToInt(itemWeight),
            .cost = doubleToInt(itemCost),
        };
    }

    return {items, doubleToInt(weight)};
}
}  // namespace

int main() {
    const auto [items, weight] = getItems();

    std::cout << std::fixed << std::setprecision(9) << intToDouble(calcMaxCost(items, weight, 0, 0.0, 0.0)) << std::endl;

    return 0;
}
