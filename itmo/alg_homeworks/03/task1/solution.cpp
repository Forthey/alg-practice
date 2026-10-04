#include "solution.h"

#include <algorithm>
#include <queue>
#include <ranges>

void sortShiftedElements(std::vector<SerialNumber>& arr, std::int64_t shift) {
    if (shift == 0 || arr.empty()) {
        return;
    }

    for (auto left : std::views::iota(arr.begin(), arr.end())) {
        const auto right = arr.end() - left > shift ? left + shift + 1 : arr.end();

        // O(k)
        auto min = std::ranges::min_element(left, right);

        std::swap(*left, *min);
    }
}

void insertionSort(std::vector<SerialNumber>& arr) {
    if (arr.size() <= 1) {
        return;
    }

    for (auto unsortedBegin : std::views::iota(arr.begin() + 1, arr.end())) {
        auto newElement = unsortedBegin;
        for (auto insertCandidate : std::views::iota(arr.begin(), unsortedBegin) | std::views::reverse) {
            if (*newElement > *insertCandidate) {
                break;
            }

            std::swap(*newElement, *insertCandidate);
            newElement = insertCandidate;
        }
    }
}

void sortShiftedElementsFast(std::vector<SerialNumber>& arr, std::int64_t shift) {
    if (shift == 0 || arr.empty()) {
        return;
    }

    shift = std::min(shift, static_cast<std::int64_t>(arr.size() - 1));

    std::priority_queue<SerialNumber, std::vector<SerialNumber>, std::greater<>> slidingWindow;

    for (auto iter : std::views::iota(arr.begin(), arr.begin() + shift + 1)) {
        slidingWindow.push(*iter);
    }

    for (auto left : std::views::iota(arr.begin(), arr.end())) {
        // O(log(k))
        auto min = slidingWindow.top();
        slidingWindow.pop();

        if (arr.end() - left > shift) {
            // O(log(k))
            slidingWindow.push(*(left + shift + 1));
        }

        std::swap(*left, min);
    }
}