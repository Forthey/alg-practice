#include <array>
#include <cstdint>
#include <iostream>

namespace {
using Number = std::uint16_t;
constexpr auto kMaxPartitionLength = 40;
using Partition = std::array<Number, kMaxPartitionLength>;

void printPartition(const Partition& partition) {
    for (const auto& number : partition) {
        if (number == 0) {
            break;
        }
        std::cout << number << " ";
    }
    std::cout << "\n";
}

void partitions(Partition& currentPartition, std::size_t currentPartitionIndex, Number remaining, Number lastElement) {
    if (remaining == 0) {
        printPartition(currentPartition);
        return;
    }

    for (auto newNumber = 1; newNumber <= std::min(lastElement, remaining); ++newNumber) {
        currentPartition[currentPartitionIndex] = newNumber;

        partitions(currentPartition, currentPartitionIndex + 1,  remaining - newNumber, newNumber);

        currentPartition[currentPartitionIndex] = 0;
    }
}

void partitions(Number number) {
    Partition beginPartition{};
    partitions(beginPartition, 0, number, number);
}
}

int main() {
    Number number;

    std::cin >> number;

    partitions(number);

    return 0;
}
