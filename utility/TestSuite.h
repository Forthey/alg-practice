#pragma once
#include <array>
#include <functional>
#include <iostream>


template<typename T>
concept Printable = requires(std::ostream& os, const T& v) {
    { os << v } -> std::same_as<std::ostream&>;
};

template<typename T>
concept Testable = Printable<T> && std::equality_comparable<T>;

template<Printable Input, Testable Result>
struct TestCase {
    Input input;
    Result expected;
};

template<std::size_t testCasesCount, Printable Input, Testable Result>
struct TestSuite {
    TestSuite(std::array<TestCase<Input, Result>, testCasesCount> testCases,
              std::function<Result(const Input &)> testFunction) : m_testCases{std::move(testCases)},
                                                                   m_testFunction{std::move(testFunction)} {
    }

    void run() {
        for (const auto& testCase : m_testCases) {
            const auto result = m_testFunction(testCase.input);
            const auto expected = testCase.expected;
            std::cout << "expected: " << testCase.expected << ", result: " << result << (result == expected ? " OK" : " FAIL") << std::endl;
        }
    }

private:
    const std::array<TestCase<Input, Result>, testCasesCount> m_testCases;
    const std::function<Result(const Input&)> m_testFunction;
};

template<std::size_t testCasesCount, Printable Input, Testable Result, typename Functor>
TestSuite(std::array<TestCase<Input, Result>, testCasesCount>, Functor) -> TestSuite<testCasesCount, Input, Result>;
