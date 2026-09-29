#include <cstdint>
#include <generator>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <variant>
#include <ranges>
#include <vector>

using Time = std::uint64_t;
using BlockId = std::uint64_t;

struct MemoryBlock {
    Time time;
    BlockId id;

    friend bool operator<(const MemoryBlock& a, const MemoryBlock& b) {
        // Инвертируем сравнение, чтобы priority_queue работала как min-heap.
        return std::tie(a.time, a.id) > std::tie(b.time, b.id);
    }
};

using MemoryBlocks = std::priority_queue<MemoryBlock>;
using FreeBlocks = std::set<BlockId>;

class MemoryBlocksManager {
public:
    explicit MemoryBlocksManager(std::size_t size, Time expires)
        : expires{expires},
          actualExpire(size + 1, 0),
          busy(size + 1, false) {

        for (const auto i : std::views::iota(0u, size)) {
            freeBlocks.insert(i + 1);
        }
    }

    BlockId allocate(Time time) {
        releaseExpired(time);

        const auto it = freeBlocks.begin();
        const BlockId id = *it;
        freeBlocks.erase(it);

        occupy(id, time);

        return id;
    }

    bool hasAccess(BlockId id, Time time) {
        releaseExpired(time);

        if (!busy[id]) {
            return false;
        }

        // Успешный доступ продлевает занятость ещё на T.
        occupy(id, time);

        return true;
    }

private:
    void occupy(BlockId id, Time time) {
        busy[id] = true;

        actualExpire[id] = time + expires;
        blocks.push({
            .time = actualExpire[id],
            .id = id
        });
    }

    void releaseExpired(Time time) {
        while (!blocks.empty() && blocks.top().time <= time) {
            const auto block = blocks.top();
            blocks.pop();

            if (!busy[block.id] ||
                actualExpire[block.id] != block.time) {
                continue;
            }

            busy[block.id] = false;
            freeBlocks.insert(block.id);
        }
    }

private:
    const Time expires;

    MemoryBlocks blocks;
    FreeBlocks freeBlocks;

    std::vector<Time> actualExpire;
    std::vector<bool> busy;
};

struct AllocateCommand {
    Time time{};
};

struct AccessCommand {
    Time time{};
    BlockId blockId{};
};

std::generator<std::variant<AllocateCommand, AccessCommand>> commands() {
    Time time{};
    char operation{};

    while (std::cin >> time >> operation) {
        BlockId blockId;

        switch (operation) {
            case '.':
                std::cin >> blockId;
                co_yield AccessCommand{time, blockId};
                break;

            case '+':
                co_yield AllocateCommand{time};
                break;
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // В условии T = 10 минут, а время дано в секундах.
    MemoryBlocksManager manager{30000, 10 * 60};

    for (const auto& command : commands()) {
        if (const auto* allocate =
                std::get_if<AllocateCommand>(&command)) {

            std::cout
                << manager.allocate(allocate->time)
                << '\n';

            continue;
        }

        const auto& [time, blockId] =
            std::get<AccessCommand>(command);

        std::cout
            << (manager.hasAccess(blockId, time) ? "+" : "-")
            << '\n';
    }

    return 0;
}