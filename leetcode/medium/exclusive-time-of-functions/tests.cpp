#include "Solution.h"
#include "gtest/gtest.h"

namespace solution {
namespace {
struct TestCase {
    int n;
    std::vector<std::string> logs;

    std::vector<int> expected;
};

TestCase testCases[] = {
    {.n = 2, .logs = {"0:start:0", "1:start:2", "1:end:5", "0:end:6"}, .expected = {3, 4}},
    {.n = 1, .logs = {"0:start:0", "0:start:2", "0:end:5", "0:start:6", "0:end:6", "0:end:7"}, .expected = {8}},
    {.n = 2, .logs = {"0:start:0", "0:start:2", "0:end:5", "1:start:6", "1:end:6", "0:end:7"}, .expected = {7, 1}},
};

class SolutionTest : public ::testing::TestWithParam<TestCase> {
protected:
    Solution solution;
};

TEST_P(SolutionTest, ReturnsExpected) {
    auto testCase = GetParam();

    EXPECT_EQ(testCase.expected, solution.exclusiveTime(testCase.n, testCase.logs));
}

INSTANTIATE_TEST_SUITE_P(SolutionTest, SolutionTest, testing::ValuesIn(testCases));
}  // namespace
}  // namespace solution