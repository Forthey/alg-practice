#include <cstddef>
#include <cstdint>
#include <iostream>
#include <queue>
#include <ranges>

namespace {
struct TaxiRate {
    std::size_t id{};
    std::uint16_t value{};

    friend bool operator<(const TaxiRate& lhs, const TaxiRate& rhs) {
        return lhs.value < rhs.value;
    }
};

struct Employee {
    std::size_t id{};
    std::uint16_t distance{};

    friend bool operator<(const Employee& lhs, const Employee& rhs) {
        return lhs.distance > rhs.distance;
    }
};

using Taxis = std::priority_queue<TaxiRate>;
using Employees = std::priority_queue<Employee>;

std::vector<std::size_t> countDistances(Taxis& taxis, Employees& employees) {
    std::vector<std::size_t> result(taxis.size());

    while (!taxis.empty()) {
        const auto& taxi = taxis.top();
        const auto& employee = employees.top();

        result[employee.id] = taxi.id;

        taxis.pop();
        employees.pop();
    }

    return result;
}

struct Input {
    Taxis taxis;
    Employees employees;
};

Input input() {
    std::size_t n;

    std::cin >> n;

    Employees employees{};

    for (const auto i : std::views::iota(0u, n)) {
        Employee employee;

        employee.id = i;
        std::cin >> employee.distance;

        employees.emplace(employee);
    }

    Taxis taxis{};

    for (const auto i : std::views::iota(0u, n)) {
        TaxiRate taxi;

        taxi.id = i + 1;
        std::cin >> taxi.value;

        taxis.emplace(taxi);
    }

    return {taxis, employees};
}
}

int main() {
    auto [taxis, employees] = input();

    for (const auto& taxiId : countDistances(taxis, employees)) {
        std::cout << taxiId << ' ';
    }
    return 0;
}
