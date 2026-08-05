#include <array>
#include <iostream>
#include <vector>

#include "StdOutput.h"
#include "TestSuite.h"

std::vector<int> mapNumbersToLessThanCount(const std::vector<int>& elements) {
    // Пара элемент + индекс из оригинального массива
    std::vector<std::pair<int, int>> sortedElementsPair;
    sortedElementsPair.reserve(elements.size());

    for (int i = 0; i < static_cast<int>(elements.size()); ++i) {
        sortedElementsPair.emplace_back(elements[i], i);
    }

    std::sort(sortedElementsPair.begin(), sortedElementsPair.end());

    std::vector<int> result(elements.size());

    // Чтобы в отсторированном массиве одинаковым X, X, X присваивать значение индекса первого элемента
    int lastEqIndex = -1;

    for (int i = 0; i < static_cast<int>(sortedElementsPair.size()); ++i) {
        const auto& [element, index] = sortedElementsPair[i];
        if (i != 0 && sortedElementsPair[i - 1].first == element) {
            if (lastEqIndex == -1) {
                lastEqIndex = i - 1;
            }
            result[index] = lastEqIndex;
            continue;
        }

        lastEqIndex = -1;
        result[index] = i;
    }

    return result;
}

struct Input {
    std::vector<int> nums;

    friend std::ostream& operator<<(std::ostream& os, const Input& input) {
        os << input.nums;
        return os;
    }
};

struct Result {
    std::vector<int> value;

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
                .input = {.nums = {8, 1, 2, 2, 3}},
                .expected = {.value = {4, 0, 1, 1, 3}},
            },
            TestCase<Input, Result>{
                .input = {.nums = {6, 5, 4, 8}},
                .expected = {.value = {2, 1, 0, 3}},
            },
            TestCase<Input, Result>{
                .input = {.nums = {7, 7, 7, 7}},
                .expected = {.value = {0, 0, 0, 0}},
            },
        },
        [](const Input& input) { return Result{.value = mapNumbersToLessThanCount(input.nums)}; },
    };

    suite.run();

    return 0;
}
