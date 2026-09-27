#include <cassert>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <stack>

#include "solution.h"

const std::map<char, char> matchMap = {
    {']', '['},
    {'}', '{'},
    {')', '('},
};

const std::set openParentheses = {'[', '{', '('};

std::string findLongestSubstring(const std::string& str) {
    std::stack<std::size_t> symbols;

    std::size_t bestBegin = 0;
    std::size_t bestLength = 0;

    std::size_t segmentBegin = 0;

    for (std::size_t i = 0; i < str.size(); ++i) {
        if (openParentheses.contains(str[i])) {
            symbols.push(i);
            continue;
        }

        const auto it = matchMap.find(str[i]);

        // Тут по факту все условия базовой "поломки":
        // либо мы получили не скобки,
        // либо у нас в целом закончили возможности найти пару слева,
        // либо пара слева не матчится с парой справа
        if (it == matchMap.end() || symbols.empty() || str[symbols.top()] != it->second) {
            // Тогда мы все это время парсили неправильную скобочную последовательность, так что чистим стек
            while (!symbols.empty()) {
                symbols.pop();
            }
            segmentBegin = i + 1;
            continue;
        }

        symbols.pop();

        // Тут идея простая: мы либо распарсили весь стек - тогда нужно смотреть на маркер начала псп
        // либо мы распарсили не весь стек - тогда segmentBegin - это самая первая открывающая скобка, для нее матча еще нет
        // поэтому нужно взять top() + 1, так как именно для нее матч только что нашелся
        std::size_t currentBegin;
        if (symbols.empty()) {
            currentBegin = segmentBegin;
        } else {
            currentBegin = symbols.top() + 1;
        }

        // Ну и тут сравниваем текущую найденную псп с лучшим результатом
        std::size_t currentLength = i - currentBegin + 1;
        if (currentLength > bestLength) {
            bestBegin = currentBegin;
            bestLength = currentLength;
        }
    }

    return str.substr(bestBegin, bestLength);
}
