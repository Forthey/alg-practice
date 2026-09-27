#pragma once

#include <algorithm>
#include <vector>

template <typename T>
int findMaxConsecutive(const std::vector<T>& nums, const T& targetElement) {
    int maxConsecutive = 0;
    int currentConsecutive = 0;

    for (const auto& num : nums) {
        if (num == targetElement) {
            currentConsecutive++;
            continue;
        }
        // Элементы не совпали
        // Пробуем переписать максимальное число вхождений и стираем факт наличия прошлого элемента
        if (currentConsecutive > maxConsecutive) {
            maxConsecutive = currentConsecutive;
        }
        currentConsecutive = 0;
    }
    return std::max(maxConsecutive, currentConsecutive);
}
