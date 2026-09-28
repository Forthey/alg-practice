// Как будто бы это нужно решать кучей, так что я решу через готовые структуры данных, если никто не против)
#include <generator>
#include <iostream>
#include <queue>
#include <set>

using Time = std::uint64_t;
using BlockId = std::uint64_t;

struct MemoryBlock {
    Time time;
    BlockId id;

    friend bool operator<(const MemoryBlock& a, const MemoryBlock& b) {
        return std::tie(a.time, a.id) < std::tie(b.time, b.id);
    }
};

using MemoryBlocks = std::priority_queue<MemoryBlock>;
using FreeBlocks = std::set<BlockId>;

class MemoryBlocksManager {
public:
    constexpr explicit MemoryBlocksManager(std::size_t size, Time expires) : expires{expires} {
        for ([[maybe_unused]] const auto i : std::views::iota(0u, size)) {
            blocks.push({.time = 0, .id = i + 1});
        }
    }

    BlockId allocate() {
        // TODO а как...
        return {};
    }

    bool hasAccess(BlockId id) {
        // TODO а как...
        return !freeBlocks.contains(id);
    }

private:
    [[maybe_unused]] const Time expires;
    MemoryBlocks blocks;
    FreeBlocks freeBlocks;
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
            default:
                break;
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    MemoryBlocksManager manager{30000, 10};

    for (const auto& command : commands()) {
        if (std::get_if<AllocateCommand>(&command)) {
            std::cout << manager.allocate() << std::endl;
            continue;
        }

        const auto& [time, blockId] = std::get<AccessCommand>(command);

        std::cout << (manager.hasAccess(blockId) ? "+" : "-") << std::endl;
    }

    return 0;
}
