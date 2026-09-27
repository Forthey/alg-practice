#include "solution.h"

std::vector<int> mapNumbersToLessThanCount(const std::vector<int>& elements) {
    // Пара элемент + индекс из оригинального массива
    std::vector<std::pair<int, int>> sortedElementsPair;
    sortedElementsPair.reserve(elements.size());

    for (int i = 0; i < static_cast<int>(elements.size()); ++i) {
        sortedElementsPair.emplace_back(elements[i], i);
    }

    std::sort(sortedElementsPair.begin(), sortedElementsPair.end());

    std::vector<int> result(elements.size());

    // Чтобы в отсторированном массиве одинаковым X, X, X присваивать значение индекса первого элемента
    int lastEqIndex = -1;

    for (int i = 0; i < static_cast<int>(sortedElementsPair.size()); ++i) {
        const auto& [element, index] = sortedElementsPair[i];
        if (i != 0 && sortedElementsPair[i - 1].first == element) {
            if (lastEqIndex == -1) {
                lastEqIndex = i - 1;
            }
            result[index] = lastEqIndex;
            continue;
        }

        lastEqIndex = -1;
        result[index] = i;
    }

    return result;
}
