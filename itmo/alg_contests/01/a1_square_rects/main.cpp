#include <iostream>
#include <generator>

struct Rectangle {
    // Левый нижний
    std::int64_t x1{};
    std::int64_t y1{};
    // Правый верхний
    std::int64_t x2{};
    std::int64_t y2{};

    [[nodiscard]] std::int64_t square() const {
        return (x2 - x1) * (y2 - y1);
    }

    [[nodiscard]] Rectangle intersect(const Rectangle& other) const {
        const Rectangle intersect{
            .x1 = std::max(x1, other.x1),
            .y1 = std::max(y1, other.y1),
            .x2 = std::min(x2, other.x2),
            .y2 = std::min(y2, other.y2)
        };

        if (intersect.x1 > intersect.x2 || intersect.y1 > intersect.y2) {
            return {};
        }

        return intersect;
    }
};

std::generator<Rectangle> rectangles() {
    std::uint16_t n;

    std::cin >> n;

    for ([[maybe_unused]] auto i : std::views::iota(0u, n)) {
        Rectangle square;

        std::cin >> square.x1 >> square.y1 >> square.x2 >> square.y2;

        co_yield square;
    }
}

int main() {
    std::optional<Rectangle> prevRectangle;

    for (const auto& rectangle : rectangles()) {
        if (!prevRectangle) {
            prevRectangle = rectangle;
            continue;
        }

        prevRectangle = prevRectangle->intersect(rectangle);

        // Небольшая оптимизация :>
        if (prevRectangle->square() == 0) {
            break;
        }
    }

    std::cout << prevRectangle->square() << std::endl;

    return 0;
}
