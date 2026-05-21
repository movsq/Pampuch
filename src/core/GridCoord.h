#pragma once

struct GridCoord {
    int x = 0;
    int y = 0;

    constexpr GridCoord() = default;
    constexpr GridCoord(int x_, int y_) : x(x_), y(y_) {}

    [[nodiscard]] constexpr bool operator==(const GridCoord& other) const noexcept {
        return x == other.x && y == other.y;
    }

    [[nodiscard]] constexpr bool operator!=(const GridCoord& other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] constexpr GridCoord operator+(const GridCoord& other) const noexcept {
        return { x + other.x, y + other.y };
    }

    [[nodiscard]] constexpr GridCoord operator-(const GridCoord& other) const noexcept {
        return { x - other.x, y - other.y };
    }
};
