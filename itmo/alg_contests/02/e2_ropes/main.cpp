#include <algorithm>
#include <iostream>
#include <vector>

using RopeLength = std::uint64_t;

struct Input {
    std::vector<RopeLength> ropes;
    std::uint32_t targetPieces{};
};

bool canSplitInto(const std::vector<RopeLength>& ropes, std::uint32_t targetPieces, RopeLength targetLength) {
    std::uint32_t currentPieces = 0;

    for (const auto& rope : ropes) {
        currentPieces += rope / targetLength;

        if (currentPieces >= targetPieces) {
            return true;
        }
    }

    return false;
}

RopeLength maximizeLength(const std::vector<RopeLength>& ropes, std::uint32_t targetPieces) {
    std::int64_t leftLength = 0;
    std::int64_t rightLength = ropes.back() + 1;

    while (rightLength - leftLength > 1) {
        const auto nextLength = leftLength + (rightLength - leftLength) / 2;

        if (canSplitInto(ropes, targetPieces, nextLength)) {
            leftLength = nextLength;
            continue;
        }

        rightLength = nextLength;
    }

    return leftLength;
}

Input input() {
    std::uint32_t n;
    std::uint32_t k;

    std::cin >> n >> k;

    Input input{
        .ropes = std::vector<RopeLength>(n),
        .targetPieces = k,
    };

    for (auto& rope : input.ropes) {
        std::cin >> rope;
    }

    return input;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto [ropes, targetPieces] = input();

    std::ranges::sort(ropes);

    std::cout << maximizeLength(ropes, targetPieces) << std::endl;

    return 0;
}
