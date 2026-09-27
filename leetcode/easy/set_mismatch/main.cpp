#include <array>
#include <iostream>
#include <ostream>
#include <vector>

#include "StdOutput.h"
#include "TestSuite.h"

std::vector<int> findDuplicate(const std::vector<int>& elements) {
    // +1, чтобы не парится с +1 дальше при обращении
    std::vector<bool> elementsRegistry(elements.size() + 1, false);
    int duplicatedElement = 0;
    ;

    for (const auto& element : elements) {
        if (elementsRegistry[element]) {
            duplicatedElement = element;
            continue;
        }
        elementsRegistry[element] = true;
    }
    for (std::size_t element = 1; element < elementsRegistry.size(); element++) {
        if (!elementsRegistry[element]) {
            return {duplicatedElement, static_cast<int>(element)};
        }
    }
    return {};
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
                .input = {.nums = {1, 2, 2, 4}},
                .expected = {.value = {2, 3}},
            },
            TestCase<Input, Result>{
                .input = {.nums = {1, 1}},
                .expected = {.value = {1, 2}},
            },
        },
        [](const Input& input) { return Result{.value = findDuplicate(input.nums)}; },
    };

    suite.run();

    return 0;
}
