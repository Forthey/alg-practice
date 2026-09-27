#include <cstdint>
#include <generator>
#include <iostream>
#include <queue>
#include <ranges>
#include <unordered_set>

using Timestamp = std::int32_t;
using ProductId = std::uint16_t;

struct Event {
    bool isHuman{};
    Timestamp timeStamp{};
    ProductId productId{};
};

std::generator<Event> events() {
    std::uint16_t n;

    std::cin >> n;

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        std::uint16_t type;
        std::int32_t timestamp;
        std::uint16_t productId;

        std::cin >> type >> timestamp >> productId;

        co_yield Event{
            .isHuman = type == 2,
            .timeStamp = timestamp,
            .productId = productId,
        };
    }
}

std::generator<Timestamp> personsAwaits() {
    std::unordered_multiset<ProductId> products;
    std::queue<Event> persons;

    for (const auto& event : events()) {
        if (event.isHuman) {
            // Человек пришел, очередь непустая или продукта нет - ожидание
            if (!persons.empty() || !products.contains(event.productId)) {
                persons.push(event);
                continue;
            }

            // Продукт есть - ура, ожидание - 0
            products.erase(products.find(event.productId));
            co_yield {};
            continue;
        }

        // Продукт пришел, но он не нужен :(. Кладем на склад
        if (persons.empty() || persons.front().productId != event.productId) {
            products.emplace(event.productId);
            continue;
        }

        // Продукт пришел и нужен клиенту (наверное). Ура!!! Ожидание - разность между приходом и этим событием
        // Возможно, кто-то еще заждался товар, так что чекаем всех в очереди до конца совпадений
        co_yield event.timeStamp - persons.front().timeStamp;
        persons.pop();

        while (!persons.empty() && products.contains(persons.front().productId)) {
            co_yield event.timeStamp - persons.front().timeStamp;
            products.erase(products.find(persons.front().productId));
            persons.pop();
        }
    }

    // Кто-то не дождался товаров :(
    while (!persons.empty()) {
        co_yield -1;
        persons.pop();
    }
}

int main() {
    for (const auto await : personsAwaits()) {
        std::cout << await << std::endl;
    }
    return 0;
}
