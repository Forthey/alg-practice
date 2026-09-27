#include <limits>
#include <ranges>
#include <vector>

#include "solution.h"

struct Segment {
    int sum;
    std::size_t l;
    std::size_t r;
};

struct Result {
    std::size_t l1;
    std::size_t l2;
    std::size_t r1;
    std::size_t r2;
};

Result findMaxPair(const std::vector<int>& a) {
    if (a.size() < 3) {
        return {};
    }

    Result result{};
    std::vector<Segment> leftMax(a.size());
    std::vector<Segment> rightMax(a.size());

    Segment currentLeft{a.front(), 0, 0};
    Segment currentRight{a.back(), a.size() - 1, a.size() - 1};

    leftMax.front() = currentLeft;
    rightMax.back() = currentRight;

    for (const auto i : std::views::iota(1u, a.size())) {
        if (currentLeft.sum + a[i] >= a[i]) {
            currentLeft.sum += a[i];
            currentLeft.r = i;
        } else {
            currentLeft = {a[i], i, i};
        }

        if (leftMax[i - 1].sum >= currentLeft.sum) {
            leftMax[i] = leftMax[i - 1];
        } else {
            leftMax[i] = currentLeft;
        }

        const auto j = a.size() - i - 1;

        if (a[j] + currentRight.sum >= a[j]) {
            currentRight.sum += a[j];
            currentRight.l = j;
        } else {
            currentRight = {a[j], j, j};
        }

        if (rightMax[j + 1].sum >= currentRight.sum) {
            rightMax[j] = rightMax[j + 1];
        } else {
            rightMax[j] = currentRight;
        }
    }

    int maxSum = std::numeric_limits<int>::min();

    for (const auto splitter : std::views::iota(1u, a.size() - 1)) {
        const auto sum = leftMax[splitter - 1].sum + rightMax[splitter + 1].sum;

        if (sum <= maxSum) {
            continue;
        }

        maxSum = sum;

        result = {
            .l1 = leftMax[splitter - 1].l,
            .l2 = rightMax[splitter + 1].l,
            .r1 = leftMax[splitter - 1].r,
            .r2 = rightMax[splitter + 1].r,
        };
    }

    return result;
}
