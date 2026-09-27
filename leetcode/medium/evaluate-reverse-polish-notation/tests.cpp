#include "Solution.h"
#include "gtest/gtest.h"

namespace solution {
namespace {
struct TestCase {
    std::vector<std::string> tokens;
    int expected;
};

TestCase testCases[] = {
    {.tokens = {"2", "1", "+", "3", "*"}, .expected = 9},
    {.tokens = {"4", "13", "5", "/", "+"}, .expected = 6},
};

class SolutionTest : public ::testing::TestWithParam<TestCase> {
protected:
    Solution solution;
};

TEST_P(SolutionTest, ReturnsExpected) {
    auto testCase = GetParam();

    EXPECT_EQ(testCase.expected, solution.evalRPN(testCase.tokens));
}

INSTANTIATE_TEST_SUITE_P(SolutionTest, SolutionTest, testing::ValuesIn(testCases));
}  // namespace
}  // namespace solution