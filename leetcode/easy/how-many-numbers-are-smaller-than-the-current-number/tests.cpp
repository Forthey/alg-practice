#include "solution.h"

#include <gtest/gtest.h>

namespace {
struct Case {
    std::vector<int> nums;
    std::vector<int> expected;
};

const Case cases[] = {
    {{8, 1, 2, 2, 3}, {4, 0, 1, 1, 3}},
    {{6, 5, 4, 8}, {2, 1, 0, 3}},
    {{7, 7, 7, 7}, {0, 0, 0, 0}},
};

using SolutionTest = testing::TestWithParam<Case>;

TEST_P(SolutionTest, ReturnsExpected) {
    const auto& c = GetParam();
    EXPECT_EQ(c.expected, mapNumbersToLessThanCount(c.nums));
}

INSTANTIATE_TEST_SUITE_P(Examples, SolutionTest, testing::ValuesIn(cases));
}  // namespace
