#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>

bool isNotBouncy(const std::uint64_t number) {
    std::optional<std::uint64_t> lastDigit;
    std::optional<bool> shouldDecreasing;

    // Тут важно, что итерация происходит от младших к старшим, то есть, new > old означает, что число decreasing
    for (auto slicedNumber = number, newDigit = slicedNumber % 10; slicedNumber > 0;
        lastDigit = newDigit, slicedNumber /= 10, newDigit = slicedNumber % 10) {
        // Первый проход
        if (!lastDigit) {
            continue;
        }

        // Второй проход, или пока цифры совпадают
        if (!shouldDecreasing) {
            if (newDigit == *lastDigit) {
                continue;
            }
            shouldDecreasing = newDigit > *lastDigit;
            continue;
        }

        // Число должно быть decreasing, но мы получили, что старшая цифра меньше младшей
        if (*shouldDecreasing && newDigit < *lastDigit) {
            return false;
        }

        // Число должно быть increasing, но мы получили, что старшная цифра больше младшей
        if (!*shouldDecreasing && newDigit > *lastDigit) {
            return false;
        }
    }
    return true;
}

std::uint64_t totalNotBouncyNumbersBad(unsigned int n) {
    static std::map<unsigned int, std::uint64_t> notBouncyNumbersCash;

    // Всегда считаем 0, который не вписывается нормально дальше(
    std::uint64_t total = 1;
    const std::uint64_t maxNumber = std::pow(10, n);
    std::uint64_t currentNumber = maxNumber;

    for (auto i = n; i > 0; --i) {
        const auto it = notBouncyNumbersCash.find(i);
        if (it == notBouncyNumbersCash.end()) {
            currentNumber /= 10;
            continue;
        }
        total = it->second;
        break;
    }

    while (currentNumber < maxNumber) {
        total += static_cast<std::uint64_t>(isNotBouncy(currentNumber));
        ++currentNumber;
    }

    notBouncyNumbersCash[n] = total;
    return total;
}

std::uint64_t totalNotBouncyNumbers(unsigned int n) {
    return n;
}

int main() {
    int input;
    for (std::cin >> input; input != -1; std::cin >> input) {
        std::cout << "NEW: " << totalNotBouncyNumbers(input) << std::endl;
        std::cout << "OLD: " << totalNotBouncyNumbersBad(input) << std::endl;
    }
    return 0;
}