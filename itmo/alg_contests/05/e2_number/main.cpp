#include <iostream>
#include <queue>

namespace {
struct String {
    std::string value;

    friend bool operator<(const String& lhs, const String& rhs) {
        return lhs.value + rhs.value < rhs.value + lhs.value;
    }
};

using Slices = std::priority_queue<String>;

void printString(Slices& slices) {
    while (!slices.empty()) {
        std::cout << slices.top().value;
        slices.pop();
    }
}

Slices getSlices() {
    Slices slices;

    std::string slice;

    while (std::cin >> slice) {
        slices.push(String{std::move(slice)});
    }

    return slices;
}
}

int main() {
    auto slices = getSlices();

    printString(slices);

    return 0;
}
