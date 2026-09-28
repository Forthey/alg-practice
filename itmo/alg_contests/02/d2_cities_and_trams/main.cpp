#include <algorithm>
#include <cstdint>
#include <generator>
#include <iostream>
#include <vector>

using Value = std::uint64_t;
using CityData = std::pair<Value, std::size_t>;

struct FindRequest {
    std::size_t minNumber{};
    std::size_t maxNumber{};
    Value targetPassengers{};
};

bool executeRequest(const std::vector<CityData>& cities, const FindRequest& request) {
    const auto mostLeftCity = std::ranges::lower_bound(cities, CityData{request.targetPassengers, request.minNumber});

    if (mostLeftCity == cities.end()) {
        return false;
    }

    const auto& [passengers, index] = *mostLeftCity;

    if (passengers != request.targetPassengers || index > request.maxNumber) {
        return false;
    }

    return true;
}

std::vector<CityData> citiesPassengers() {
    std::uint32_t n;
    std::cin >> n;

    std::vector<CityData> result(n);

    for ([[maybe_unused]] const auto i : std::views::iota(0u, n)) {
        std::cin >> result[i].first;
        result[i].second = i + 1;
    }

    return result;
}

std::generator<FindRequest> findRequests() {
    std::uint32_t q;
    std::cin >> q;

    for ([[maybe_unused]] auto i : std::views::iota(0u, q)) {
        FindRequest request;

        std::cin >> request.minNumber >> request.maxNumber >> request.targetPassengers;

        co_yield request;
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto cities = citiesPassengers();

    std::ranges::sort(cities);

    for (const auto& request : findRequests()) {
        std::cout << executeRequest(cities, request);
    }

    std::cout << std::endl;

    return 0;
}
