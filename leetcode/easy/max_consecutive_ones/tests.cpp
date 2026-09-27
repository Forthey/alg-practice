#include "solution.h"

#include <gtest/gtest.h>

namespace {
struct Case {
    std::vector<int> nums;
    int expected;
};

const Case cases[] = {
    {{1, 1, 0, 1, 1, 1}, 3},
    {{1, 0, 1, 1, 0, 1}, 2},
};

using SolutionTest = testing::TestWithParam<Case>;

TEST_P(SolutionTest, ReturnsExpected) {
    const auto& c = GetParam();
    EXPECT_EQ(c.expected, findMaxConsecutive<int>(c.nums, 1));
}

INSTANTIATE_TEST_SUITE_P(Examples, SolutionTest, testing::ValuesIn(cases));
}  // namespace
