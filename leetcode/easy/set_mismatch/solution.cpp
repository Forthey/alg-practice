#include "solution.h"

std::vector<int> findDuplicate(const std::vector<int>& elements) {
    // +1, чтобы не парится с +1 дальше при обращении
    std::vector<bool> elementsRegistry(elements.size() + 1, false);
    int duplicatedElement = 0;
    ;

    for (const auto& element : elements) {
        if (elementsRegistry[element]) {
            duplicatedElement = element;
            continue;
        }
        elementsRegistry[element] = true;
    }
    for (std::size_t element = 1; element < elementsRegistry.size(); element++) {
        if (!elementsRegistry[element]) {
            return {duplicatedElement, static_cast<int>(element)};
        }
    }
    return {};
}
