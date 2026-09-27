#include <gtest/gtest.h>

namespace {
// Adapt the fields and examples to the solution's inputs and return type.
struct Case {
    int input;
    int expected;
};

const Case cases[] = {
    {0, 0},
};

using SolutionTest = testing::TestWithParam<Case>;

TEST_P(SolutionTest, ReturnsExpected) {
    // Copy GetParam() when the solution takes mutable arguments.
    // auto c = GetParam();
    // EXPECT_EQ(c.expected, solve(c.input));
    GTEST_SKIP() << "Replace this placeholder with a solution call and an assertion.";
}

INSTANTIATE_TEST_SUITE_P(Examples, SolutionTest, testing::ValuesIn(cases));
}  // namespace
