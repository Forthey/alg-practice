#include "solution.h"

#include <gtest/gtest.h>

namespace {
struct Case {
    std::vector<int> nums;
    std::vector<int> expected;
};

const Case cases[] = {
    {{1, 2, 2, 4}, {2, 3}},
    {{1, 1}, {1, 2}},
};

using SolutionTest = testing::TestWithParam<Case>;

TEST_P(SolutionTest, ReturnsExpected) {
    const auto& c = GetParam();
    EXPECT_EQ(c.expected, findDuplicate(c.nums));
}

INSTANTIATE_TEST_SUITE_P(Examples, SolutionTest, testing::ValuesIn(cases));
}  // namespace
