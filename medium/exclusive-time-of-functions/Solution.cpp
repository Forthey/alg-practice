#include "Solution.h"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <stack>

namespace solution {
struct FunctionData {
    std::int32_t id;
    std::uint64_t startTimestamp;
    std::int32_t sleepDuration;
};

std::vector<int> Solution::exclusiveTime(int n, std::vector<std::string>& logs) {
    std::vector<std::int32_t> result(n);
    std::stack<FunctionData> functionStack;

    for (const auto& log : logs) {
        // Тут забиваем на валидацию, так как обещают сразу корректный формат
        const auto firstColon = log.find(':');
        const auto secondColon = log.find(':', firstColon + 1);

        const auto timestampStartIndex = secondColon + 1;
        const auto operationLength = secondColon - (firstColon + 1);

        std::int32_t id;
        std::uint64_t timestamp;

        std::from_chars(log.data(), log.data() + firstColon, id);
        std::from_chars(log.data() + timestampStartIndex, log.data() + log.size(), timestamp);

        // Небольшой чит, так как мы знаем, что операций 2 и они разной длины. Это длина start
        if (operationLength == 5) {
            functionStack.push(FunctionData{
                .id = id,
                .startTimestamp = timestamp,
                .sleepDuration = 0,
            });
            continue;
        }

        const auto functionData = functionStack.top();
        functionStack.pop();
        if (functionData.id != id) {
            std::cout << "!!!!! Stack is corrupted !!!!!" << std::endl;
            return {};
        }
        const auto functionOnStackDuration = static_cast<int>(timestamp - functionData.startTimestamp + 1);
        result[id] += functionOnStackDuration - functionData.sleepDuration;

        if (functionStack.empty()) {
            continue;
        }

        auto& parentFunctionData = functionStack.top();
        parentFunctionData.sleepDuration += functionOnStackDuration;
    }

    return result;
}
}  // namespace solution
