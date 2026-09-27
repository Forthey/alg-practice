#include "solution.h"

#include <gtest/gtest.h>

namespace {
struct TestCase {
    std::string input;
    std::string expected;
};

void PrintTo(const TestCase& p, std::ostream* os) {
    *os << "\'input= " << p.input << "\'";
}

TestCase testCases[] = {
    {"", ""},
    {"(", ""},
    {")", ""},
    {"()", "()"},
    {"[]", "[]"},
    {"{}", "{}"},

    {"()()", "()()"},
    {"[]{}()", "[]{}()"},
    {"()[]{}", "()[]{}"},

    {"(())", "(())"},
    {"([])", "([])"},
    {"{[()]}", "{[()]}"},

    {"(]", ""},
    {"[)", ""},
    {"{]", ""},

    {"([)]", ""},
    {"[(])", ""},
    {"{[}]", ""},

    {")()", "()"},
    {"]([])", "([])"},
    {"}()[]", "()[]"},

    {"(()", "()"},
    {"((()))(", "((()))"},
    {"([]){", "([])"},

    {"()](())", "(())"},
    {"([])]{}()", "([])"},
    {"(]{}[]()", "{}[]()"},

    {"()](())[]", "(())[]"},
    {"[])(())", "(())"},

    {"[()]", "[()]"},
    {"{()}[()](()[])", "{()}[()](()[])"}
};

class SolutionTest : public ::testing::TestWithParam<TestCase> {
protected:
};

TEST_P(SolutionTest, buildArray_Parametrized_ReturnsExpectedValue) {
    const auto& [input, expected] = GetParam();

    EXPECT_EQ(expected, findLongestSubstring(input));
}

INSTANTIATE_TEST_SUITE_P(SolutionTest, SolutionTest, testing::ValuesIn(testCases));
}  // namespace
