#include "solution.h"

std::vector<int> findAllMissingNumbers(const std::vector<int>& elements) {
    std::vector<bool> isElementPreserved(elements.size() + 1, false);

    for (const auto& element : elements) {
        isElementPreserved[element] = true;
    }

    std::vector<int> missingElements;

    for (int i = 1; i < static_cast<int>(isElementPreserved.size()); i++) {
        if (!isElementPreserved[i]) {
            missingElements.push_back(i);
        }
    }

    return missingElements;
}
