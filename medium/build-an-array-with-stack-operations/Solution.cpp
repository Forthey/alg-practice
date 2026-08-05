#include "Solution.h"

#include <optional>

namespace solution {
std::vector<std::string> Solution::buildArray(std::vector<int>& target, int n) {
    // Идея алгоритма - идем в цикле по стриму
    // Если element < currentStream - ошибка, такого быть не может.
    // Если element == currentStream и смещения нет - делаем push
    // Если element == currentStream и смещение есть - Pop всех элементов до элемента, который указан как начало смещения, делаем push, обнуляем смещение
    // Если element > currentStream, то делаем push и добавляем факт смещения, если он еще не задан
    // 1, 2, 5 -> Push, Push, Push, Push, Pop, Pop, Push

    std::optional<int> offsetBeginNumber;
    auto element = target.begin();
    std::vector<std::string> result;

    for (int currentStream = 1; currentStream <= n && element != target.end(); ++currentStream) {
        if (*element < currentStream) {
            // Какая-то фигня - выход с {}
            return {};
        }

        if (*element > currentStream) {
            result.emplace_back(kPush);
            if (!offsetBeginNumber) {
                offsetBeginNumber = currentStream;
            }
            continue;
        }

        // *element == currentStream

        if (offsetBeginNumber) {
            const auto popCount = *element - *offsetBeginNumber;
            for (int i = 0; i < popCount; ++i) {
                result.emplace_back(kPop);
            }
            offsetBeginNumber = std::nullopt;
        }

        result.emplace_back(kPush);
        ++element;
    }

    return result;
}
}  // namespace solution
