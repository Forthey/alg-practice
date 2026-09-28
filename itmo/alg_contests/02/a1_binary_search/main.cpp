#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

enum class Value { Zero, One };

std::pair<std::int64_t, std::int64_t> searchBinary(const std::vector<Value>& array) {
    std::pair<std::int64_t, std::int64_t> result{-1, array.size()};

    while (result.second - result.first > 1) {
        const auto center = result.first + (result.second - result.first) / 2;

        switch (array[center]) {
            case Value::Zero:
                result.first = center;
                break;
            case Value::One:
                result.second = center;
                break;
        }
    }

    return result;
}

std::vector<Value> inputValues() {
    std::uint32_t n;
    std::cin >> n;

    std::vector<Value> result;
    result.reserve(n);

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        char value;
        std::cin >> value;

        result.emplace_back(value == '0' ? Value::Zero : Value::One);
    }

    return result;
}

int main() {
    const auto result = searchBinary(inputValues());

    std::cout << result.first << " " << result.second << std::endl;

    return 0;
}
