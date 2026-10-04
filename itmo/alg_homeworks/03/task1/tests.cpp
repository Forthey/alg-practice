#include <gtest/gtest.h>

#include "solution.h"

namespace {
// Adapt the fields and examples to the solution's inputs and return type.
struct Case {
    std::vector<SerialNumber> input;
    std::int64_t shift;
    std::vector<SerialNumber> expected;
};

const Case cases[] = {
    {
        .input = {4, 1, 3, 5, 2, 6, 7, 10, 9, 8},
        .shift = 3,
        .expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    },
    {
        .input = {},
        .shift = 3,
        .expected = {},
    },
    {
        .input = {2, 1},
        .shift = 5,
        .expected = {1, 2},
    },
    {
        .input = {1, 2},
        .shift = 0,
        .expected = {1, 2},
    },
};

using SolutionTest = testing::TestWithParam<Case>;

TEST_P(SolutionTest, ReturnsExpected) {
    const auto& testCase = GetParam();

    auto testArray = GetParam().input;
    sortShiftedElements(testArray, testCase.shift);
    ASSERT_EQ(testArray, testCase.expected);

    testArray = GetParam().input;
    insertionSort(testArray);
    ASSERT_EQ(testArray, testCase.expected);

    testArray = GetParam().input;
    sortShiftedElementsFast(testArray, testCase.shift);
    ASSERT_EQ(testArray, testCase.expected);
}

INSTANTIATE_TEST_SUITE_P(Examples, SolutionTest, testing::ValuesIn(cases));
}  // namespace
