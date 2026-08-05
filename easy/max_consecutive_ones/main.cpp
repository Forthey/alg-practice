#include <array>
#include <iostream>
#include <ostream>
#include <vector>

#include "StdOutput.h"
#include "TestSuite.h"

template <typename T>
int findMaxConsecutive(const std::vector<T>& nums, const T& targetElement) {
    int maxConsecutive = 0;
    int currentConsecutive = 0;

    for (const auto& num : nums) {
        if (num == targetElement) {
            currentConsecutive++;
            continue;
        }
        // Элементы не совпали
        // Пробуем переписать максимальное число вхождений и стираем факт наличия прошлого элемента
        if (currentConsecutive > maxConsecutive) {
            maxConsecutive = currentConsecutive;
        }
        currentConsecutive = 0;
    }
    return std::max(maxConsecutive, currentConsecutive);
}

struct Input {
    std::vector<int> nums;

    friend std::ostream& operator<<(std::ostream& os, const Input& input) {
        os << input.nums;
        return os;
    }
};

struct Result {
    int value{};

    friend std::ostream& operator<<(std::ostream& os, const Result& expected) {
        os << expected.value;
        return os;
    }
    friend bool operator==(const Result& lhs, const Result& rhs) = default;
};

int main() {
    TestSuite suite{
        std::array{
            TestCase<Input, Result>{
                .input = {.nums = {1, 1, 0, 1, 1, 1}},
                .expected = {.value = 3},
            },
            TestCase<Input, Result>{
                .input = {.nums = {1, 0, 1, 1, 0, 1}},
                .expected = {.value = 2},
            },
        },
        [](const Input& input) { return Result{.value = findMaxConsecutive<int>(input.nums, 1)}; },
    };

    suite.run();

    return 0;
}
