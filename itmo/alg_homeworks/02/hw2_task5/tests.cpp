#include "solution.h"

#include <gtest/gtest.h>

namespace {
struct Case {
    std::vector<int> nums;
    int expectedSum;
};

const Case cases[] = {
    {{5, 0, 7}, 12},
    {{5, 6, 7}, 12},
    {{1, 2, -100, 4, 5}, 12},
    {{-8, -3, -6, -2, -5, -4}, -5},
    {{10, -100, 1, 2, -100, 20}, 30},
    {{0, 0, 0}, 0},
};

using PairTest = testing::TestWithParam<Case>;

TEST_P(PairTest, ReturnsSeparatedSegmentsWithMaximumSum) {
    const auto& c = GetParam();
    const auto result = findMaxPair(c.nums);
    ASSERT_LE(result.l1, result.r1);
    ASSERT_LE(result.l2, result.r2);
    ASSERT_LT(result.r1 + 1, result.l2);
    ASSERT_LT(result.r2, c.nums.size());

    int sum = 0;
    for (auto i = result.l1; i <= result.r1; ++i) {
        sum += c.nums[i];
    }
    for (auto i = result.l2; i <= result.r2; ++i) {
        sum += c.nums[i];
    }
    EXPECT_EQ(c.expectedSum, sum);
}

INSTANTIATE_TEST_SUITE_P(Examples, PairTest, testing::ValuesIn(cases));
}  // namespace
