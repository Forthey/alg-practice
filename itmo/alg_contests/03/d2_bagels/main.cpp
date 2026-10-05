#include <iostream>
#include <queue>
#include <ranges>
#include <vector>

using CoinVal = std::uint64_t;

std::vector<CoinVal> minCoinNumberForSum(std::priority_queue<CoinVal>& denominations, CoinVal targetSum) {
    std::vector<CoinVal> coins;

    while (targetSum > 0) {
        while (targetSum >= denominations.top()) {
            targetSum -= denominations.top();
            coins.push_back(denominations.top());
        }
        denominations.pop();
    }

    return coins;
}

struct Input {
    CoinVal targetSum;
    std::priority_queue<CoinVal> denominations;
};

class IOWrapper {
public:
    explicit IOWrapper(std::istream& in, std::ostream& out) : m_in{in}, m_out{out} {}

    [[nodiscard]] Input input() const {
        std::uint16_t coinCount;
        CoinVal targetSum;

        m_in >> coinCount >> targetSum;

        std::vector<CoinVal> coinsData;
        coinsData.reserve(coinCount);

        Input input{
            .targetSum = targetSum,
            .denominations{std::less<CoinVal>{}, std::move(coinsData)},
        };

        for ([[maybe_unused]] auto i : std::views::iota(std::uint16_t{}, coinCount)) {
            CoinVal coin;
            m_in >> coin;
            input.denominations.push(coin);
        }

        return input;
    }

    void setAnswer(const std::vector<CoinVal>& coins) const {
        m_out << coins.size() << std::endl;

        for (const auto& coin : coins) {
            m_out << coin << " ";
        }
        std::cout << std::endl;
    }

private:
    std::istream& m_in;
    std::ostream& m_out;
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    IOWrapper io{std::cin, std::cout};

    auto [targetSum, denominations] = io.input();

    io.setAnswer(minCoinNumberForSum(denominations, targetSum));

    return 0;
}
