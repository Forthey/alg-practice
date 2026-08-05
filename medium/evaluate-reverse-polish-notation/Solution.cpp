#include "Solution.h"

#include <cstdint>
#include <optional>
#include <stack>

namespace solution {
std::pair<std::int32_t, std::int32_t> popTwoNumbers(std::stack<std::int32_t>& numStack) {
    auto second = numStack.top();
    numStack.pop();
    auto first = numStack.top();
    numStack.pop();
    return std::make_pair(first, second);
}

int Solution::evalRPN(std::vector<std::string>& tokens) {
    std::stack<std::int32_t> numStack;

    for (const auto& token : tokens) {
        switch (token.length()) {
            case 1:
                switch (token[0]) {
                    case '+': {
                        const auto [first, second] = popTwoNumbers(numStack);
                        numStack.emplace(first + second);
                        break;
                    } case '-': {
                        const auto [first, second] = popTwoNumbers(numStack);
                        numStack.emplace(first - second);
                        break;
                    } case '*': {
                        const auto [first, second] = popTwoNumbers(numStack);
                        numStack.emplace(first * second);
                        break;
                    } case '/': {
                        const auto [first, second] = popTwoNumbers(numStack);
                        numStack.emplace(first / second);
                        break;
                    } default:
                        numStack.emplace(token[0] - '0');
                        break;
                }
                break;
            default:
                numStack.emplace(std::stoi(token));
                break;
        }
    }

    return numStack.top();
}
}  // namespace solution
