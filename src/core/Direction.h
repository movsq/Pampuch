#pragma once

#include "raylib.h"
#include "GridCoord.h"
#include <cstdint>

// Cardinal direction. The integer values are LOAD-BEARING — they match the
// original Pampuch ghost-AI direction codes (1=Right, 2=Down, 3=Left,
// 4=Up). Do not renumber: GhostAI.cpp depends on the >= / > comparisons
// behaving identically.
enum class Direction : uint8_t {
    None  = 0,
    Right = 1,
    Down  = 2,
    Left  = 3,
    Up    = 4
};

[[nodiscard]] constexpr GridCoord DirectionToOffset(Direction dir) noexcept {
    switch (dir) {
        case Direction::Right: return { 1, 0 };
        case Direction::Down:  return { 0, 1 };
        case Direction::Left:  return { -1, 0 };
        case Direction::Up:    return { 0, -1 };
        default: return { 0, 0 };
    }
}

[[nodiscard]] constexpr Vector2 DirectionToVector(Direction dir) noexcept {
    switch (dir) {
        case Direction::Right: return { 1.0f, 0.0f };
        case Direction::Down:  return { 0.0f, 1.0f };
        case Direction::Left:  return { -1.0f, 0.0f };
        case Direction::Up:    return { 0.0f, -1.0f };
        default: return { 0.0f, 0.0f };
    }
}

[[nodiscard]] inline Direction VectorToDirection(Vector2 vec) noexcept {
    if (vec.x > 0) return Direction::Right;
    if (vec.x < 0) return Direction::Left;
    if (vec.y > 0) return Direction::Down;
    if (vec.y < 0) return Direction::Up;
    return Direction::None;
}

[[nodiscard]] constexpr float DirectionToAngle(Direction dir) noexcept {
    switch (dir) {
        case Direction::Right: return 0.0f;
        case Direction::Down:  return 90.0f;
        case Direction::Left:  return 180.0f;
        case Direction::Up:    return 270.0f;
        default: return 0.0f;
    }
}
