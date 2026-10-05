#include <iostream>
#include <ranges>
#include <vector>

namespace {
using Swaps = std::vector<std::pair<std::size_t, std::size_t>>;

using MinHeap = std::vector<std::uint16_t>;

void siftDown(MinHeap& heap, std::size_t index, Swaps& swaps) {
    while (2 * index + 1 < heap.size()) {
        const auto leftChildIndex = 2 * index + 1;
        const auto rightChildIndex = 2 * index + 2;
        auto nextIndex = leftChildIndex;

        if (rightChildIndex < heap.size() && heap[rightChildIndex] < heap[leftChildIndex]) {
            nextIndex = rightChildIndex;
        }

        if (heap[nextIndex] >= heap[index]) {
            break;
        }

        std::swap(heap[index], heap[nextIndex]);
        swaps.emplace_back(index, nextIndex);

        index = nextIndex;
    }
}

Swaps fixHeap(MinHeap heap) {
    Swaps swaps;

    for (auto i : std::views::iota(std::size_t{}, heap.size() / 2) | std::views::reverse) {
        siftDown(heap, i, swaps);
    }

    return swaps;
}

class IOWrapper {
public:
    explicit IOWrapper(std::istream& in, std::ostream& out) : m_in{in}, m_out{out} {}

    [[nodiscard]] std::vector<std::uint16_t> heap() const {
        std::uint16_t n;

        m_in >> n;

        std::vector<std::uint16_t> arr(n);

        for (auto i : std::views::iota(std::uint16_t{}, n)) {
            m_in >> arr[i];
        }

        return arr;
    }

    void setAnswer(const Swaps& swaps) const {
        m_out << swaps.size() << std::endl;

        for (const auto& [i, j] : swaps) {
            m_out << i + 1 << " " << j + 1 << std::endl;
        }
    }

private:
    std::istream& m_in;
    std::ostream& m_out;
};
}


int main() {
    IOWrapper io{std::cin, std::cout};

    io.setAnswer(fixHeap(io.heap()));

    return 0;
}
