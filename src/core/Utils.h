#pragma once

#include "raylib.h"
#include "raymath.h"

namespace Utils {

    [[nodiscard]] inline Vector2 RotatePoint(Vector2 point, Vector2 pivot, float angleDeg) noexcept {
        return Vector2Add(pivot, Vector2Rotate(Vector2Subtract(point, pivot), angleDeg * DEG2RAD));
    }

    [[nodiscard]] inline float RandomFloat(float min, float max) noexcept {
        return min + static_cast<float>(GetRandomValue(0, 1000)) / 1000.0f * (max - min);
    }

}
