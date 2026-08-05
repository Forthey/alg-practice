#include "Solution.h"
#include "gtest/gtest.h"

namespace solution {
namespace {
struct TestCase {
    std::vector<int> target;
    int n;
    std::vector<std::string> expected;
};

TestCase testCases[] = {
    {.target = {1, 3}, .n = 3, .expected = {kPush, kPush, kPop, kPush}},
    {.target = {1, 2, 3}, .n = 3, .expected = {kPush, kPush, kPush}},
    {.target = {1, 2}, .n = 4, .expected = {kPush, kPush}},
    {.target = {2, 3, 4}, .n = 4, .expected = {kPush, kPop, kPush, kPush, kPush}},
};

class SolutionTest : public ::testing::TestWithParam<TestCase> {
protected:
    Solution solution;
};

TEST_P(SolutionTest, buildArray_Parametrized_ReturnsExpectedValue) {
    auto testCase = GetParam();

    EXPECT_EQ(testCase.expected, solution.buildArray(testCase.target, testCase.n));
}

INSTANTIATE_TEST_SUITE_P(SolutionTest, SolutionTest, testing::ValuesIn(testCases));
}
}