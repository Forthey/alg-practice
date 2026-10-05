#include <iostream>
#include <ranges>
#include <vector>

namespace {
std::uint16_t countComparisons(std::vector<std::uint16_t> array) {
    if (array.size() <= 1) {
        return 0;
    }

    std::uint16_t count = 0;

    for (auto unsortedBegin : std::views::iota(array.begin() + 1, array.end())) {
        auto newElement = unsortedBegin;
        for (auto insertCandidate : std::views::iota(array.begin(), unsortedBegin) | std::views::reverse) {
            ++count;
            if (*newElement >= *insertCandidate) {
                break;
            }

            std::swap(*newElement, *insertCandidate);
            newElement = insertCandidate;
        }
    }

    return count;
}

class IOWrapper {
public:
    explicit IOWrapper(std::istream& in, std::ostream& out) : m_in{in}, m_out{out} {}

    [[nodiscard]] std::vector<std::uint16_t> array() const {
        std::uint16_t n;

        m_in >> n;

        std::vector<std::uint16_t> arr(n);

        for (auto i : std::views::iota(std::uint16_t{}, n)) {
            m_in >> arr[i];
        }

        return arr;
    }

    void setAnswer(std::uint32_t answer) const {
        m_out << answer << std::endl;
    }

private:
    std::istream& m_in;
    std::ostream& m_out;
};
}

int main() {
    IOWrapper io{std::cin, std::cout};

    io.setAnswer(countComparisons(io.array()));

    return 0;
}
