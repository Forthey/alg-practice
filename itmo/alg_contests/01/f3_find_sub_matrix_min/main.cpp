#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

// Массив строк бтв
using Matrix = std::vector<std::vector<std::int64_t>>;

[[nodiscard]] Matrix calcMinimumsInSubMatrices(const Matrix& matrix, std::uint16_t l) {
    Matrix result;
    result.reserve(matrix.size() - l + 1);

    result.emplace_back();
    result[0].emplace_back(std::numeric_limits<std::int64_t>::max());

    for (const auto i : std::views::iota(0u, l)) {
        for (const auto j : std::views::iota(0u, l)) {
            if (matrix[i][j] < result[0][0]) {
                result[0][0] = matrix[i][j];
            }
        }
    }

    for (const auto i : std::views::iota(0u, l)) {
        result[0].emplace_back(std::numeric_limits<std::int64_t>::max());
        for (const auto j : std::views::iota(l , matrix.size())) {
            if (result[0][j] )
        }
    }

    return result;
}

[[nodiscard]] std::pair<Matrix, std::uint16_t> readMatrix() {
    std::uint16_t n;
    std::uint16_t l;

    std::cin >> n >> l;

    Matrix matrix;
    matrix.reserve(n);

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        matrix.emplace_back();
        for ([[maybe_unused]] auto j : std::views::iota(0u, n)) {
            std::int64_t number;

            std::cin >> number;

            matrix[i].emplace_back(number);
        }
    }

    return {matrix, l};
}

int main() {
    const auto [matrix, l] = readMatrix();

    const auto result = calcMinimumsInSubMatrices(matrix, l);

    for (const auto& row : result) {
        for (const auto& col : row) {
            std::cout << col << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
