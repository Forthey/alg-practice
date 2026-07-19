#include <iostream>
#include <vector>
#include <array>

#include "StdOutput.h"
#include "TestSuite.h"

std::vector<int> findAllMissingNumbers(const std::vector<int> &elements) {
    std::vector<bool> isElementPreserved(elements.size() + 1, false);

    for (const auto &element : elements) {
        isElementPreserved[element] = true;
    }

    std::vector<int> missingElements;

    for (int i = 1; i < static_cast<int>(isElementPreserved.size()); i++) {
        if (!isElementPreserved[i]) {
            missingElements.push_back(i);
        }
    }

    return missingElements;
}

struct Input {
    std::vector<int> nums;

    friend std::ostream &operator<<(std::ostream &os, const Input &input) {
        os << input.nums;
        return os;
    }
};

struct Result {
    std::vector<int> value;

    friend std::ostream &operator<<(std::ostream &os, const Result &expected) {
        os << expected.value;
        return os;
    }

    friend bool operator==(const Result &lhs, const Result &rhs) = default;
};

int main() {
    TestSuite suite{
        std::array{
            TestCase<Input, Result>{.input = {.nums = {4,3,2,7,8,2,3,1}}, .expected = {.value = {5, 6}},},
            TestCase<Input, Result>{.input = {.nums = {1, 1}}, .expected = {.value = {2}},},
        },
        [](const Input &input) {
            return Result{.value = findAllMissingNumbers(input.nums)};
        },
    };

    suite.run();

    return 0;
}
