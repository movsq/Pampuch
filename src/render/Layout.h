#pragma once

#include "raylib.h"
#include "core/GridCoord.h"

class Layout {
public:
    enum class HudMode {
        TopBar,
        Sidebar
    };

    void Update(int screenW, int screenH) noexcept;
    void SetHudMode(HudMode mode) noexcept;

    [[nodiscard]] Vector2 GetScreenPos(float gx, float gy) const noexcept;
    [[nodiscard]] Vector2 GetScreenPos(GridCoord coord) const noexcept;
    [[nodiscard]] Vector2 GetGridCoord(Vector2 screenPos) const noexcept;

    [[nodiscard]] int Scale() const noexcept { return m_scale; }
    [[nodiscard]] float GridStartX() const noexcept { return m_gridStartX; }
    [[nodiscard]] float GridStartY() const noexcept { return m_gridStartY; }
    [[nodiscard]] float SidebarWidth() const noexcept { return m_sidebarWidth; }
    [[nodiscard]] float HudBarHeight() const noexcept { return m_hudBarHeight; }
    [[nodiscard]] HudMode GetHudMode() const noexcept { return m_mode; }
    [[nodiscard]] float GetScaledCellSize() const noexcept;
    [[nodiscard]] float GetScaledValue(float value) const noexcept;

private:
    int m_scale = 1;
    float m_gridStartX = 0.0f;
    float m_gridStartY = 0.0f;
    float m_sidebarWidth = 0.0f;
    float m_hudBarHeight = 0.0f;
    int m_lastScreenW = 0;
    int m_lastScreenH = 0;
    HudMode m_mode = HudMode::TopBar;
};
