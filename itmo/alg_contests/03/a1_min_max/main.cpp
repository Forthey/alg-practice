#include <cstdint>
#include <generator>
#include <iostream>
#include <optional>
#include <set>

namespace {
template <typename T>
concept HeapElementT = requires(T a, T b) { a < b; } && std::copyable<T>;

template <HeapElementT Element>
class MinMaxHeap {
    struct Data {
        Element value;
        std::uint64_t count;

        friend bool operator<(const Data& lhs, const Data& rhs) { return lhs.value < rhs.value; }
    };

public:
    void insert(const Element& value) {
        const Data data = {.value = value, .count = 1ull};

        auto pair = m_data.find(data);

        if (pair != m_data.end()) {
            const Data newData = {.value = pair->value, .count = pair->count + 1};
            m_data.erase(*pair);
            m_data.insert(newData);
            return;
        }

        m_data.insert(data);
    }

    std::optional<Element> popMin() {
        auto pair = m_data.begin();
        if (pair == m_data.end()) {
            return std::nullopt;
        }

        return pop(pair);
    }

    std::optional<Element> popMax() {
        if (m_data.empty()) {
            return std::nullopt;
        }

        auto pair = std::prev(m_data.end());

        return pop(pair);
    }

private:
    Element pop(auto& pair) {

        const Data newData = {.value = pair->value, .count = pair->count - 1};

        m_data.erase(pair);

        if (newData.count > 0) {
            m_data.insert(newData);
        }

        return newData.value;
    }

    std::set<Data> m_data;
};

using ValueT = std::int64_t;

struct InsertCommand {
    ValueT value;
};

struct GetMinCommand {};

struct GetMaxCommand {};

using Command = std::variant<InsertCommand, GetMinCommand, GetMaxCommand>;

std::generator<Command> commands() {
    static constexpr auto kActionLength = 6;
    static constexpr auto kParenthesisSkip = 1;

    std::uint32_t n;

    std::cin >> n;

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        std::string command;
        std::cin >> command;

        std::string_view action{
            command.data(),
            command.data() + kActionLength,
        };

        if (action == "GetMin") {
            co_yield GetMinCommand{};
            continue;
        }

        if (action == "GetMax") {
            co_yield GetMaxCommand{};
            continue;
        }

        if (action == "Insert") {
            std::string_view numberStr{
                command.data() + kActionLength + kParenthesisSkip,
                command.size() - kParenthesisSkip,
            };
            ValueT value;

            std::from_chars(numberStr.begin(), numberStr.end(), value);

            co_yield InsertCommand{.value = value};
            continue;
        }

        std::cerr << "Chto vi mne dali" << std::endl;
    }
}
}  // namespace

int main() {
    MinMaxHeap<ValueT> minMaxHeap;

    for (const auto& cmd : commands()) {
        if (auto* command = std::get_if<InsertCommand>(&cmd)) {
            minMaxHeap.insert(command->value);
            continue;
        }

        if ([[maybe_unused]] auto* command = std::get_if<GetMinCommand>(&cmd)) {
            std::cout << *minMaxHeap.popMin() << std::endl;
            continue;
        }

        if ([[maybe_unused]] auto* command = std::get_if<GetMaxCommand>(&cmd)) {
            std::cout << *minMaxHeap.popMax() << std::endl;
            continue;
        }

        break;
    }

    return 0;
}
