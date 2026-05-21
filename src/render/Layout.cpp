#include "Layout.h"
#include "core/Config.h"

void Layout::Update(int screenW, int screenH) noexcept {
    m_lastScreenW = screenW;
    m_lastScreenH = screenH;

    const int targetHeight = (Config::Grid::ROWS * Config::Grid::CELL_SIZE) + (Config::Grid::MARGIN * 2);
    m_scale = screenH / targetHeight;
    if (m_scale < 1) m_scale = 1;

    m_sidebarWidth = static_cast<float>(Config::Grid::BASE_SIDEBAR_WIDTH * m_scale);
    m_hudBarHeight = static_cast<float>(Config::UI::HUD_BAR_BASE_HEIGHT * m_scale);

    const float gridW = static_cast<float>(Config::Grid::COLS * Config::Grid::CELL_SIZE * m_scale);
    const float gridH = static_cast<float>(Config::Grid::ROWS * Config::Grid::CELL_SIZE * m_scale);

    if (m_mode == HudMode::Sidebar) {
        const float availableWidth = screenW - m_sidebarWidth;
        if (availableWidth < gridW) {
            m_gridStartX = m_sidebarWidth + static_cast<float>(Config::Grid::MARGIN * m_scale);
        } else {
            m_gridStartX = m_sidebarWidth + (availableWidth - gridW) / 2.0f;
        }
        m_gridStartY = (screenH - gridH) / 2.0f;
    } else {
        m_gridStartX = (screenW - gridW) / 2.0f;
        if (m_gridStartX < static_cast<float>(Config::Grid::MARGIN * m_scale)) {
            m_gridStartX = static_cast<float>(Config::Grid::MARGIN * m_scale);
        }
        const float availableHeight = screenH - m_hudBarHeight;
        m_gridStartY = m_hudBarHeight + (availableHeight - gridH) / 2.0f;
        if (m_gridStartY < m_hudBarHeight) {
            m_gridStartY = m_hudBarHeight;
        }
    }
}

void Layout::SetHudMode(HudMode mode) noexcept {
    if (m_mode == mode) return;
    m_mode = mode;
    if (m_lastScreenW > 0 && m_lastScreenH > 0) {
        Update(m_lastScreenW, m_lastScreenH);
    }
}

Vector2 Layout::GetScreenPos(float gx, float gy) const noexcept {
    return {
        m_gridStartX + (gx * Config::Grid::CELL_SIZE * m_scale),
        m_gridStartY + (gy * Config::Grid::CELL_SIZE * m_scale)
    };
}

Vector2 Layout::GetScreenPos(GridCoord coord) const noexcept {
    return GetScreenPos(static_cast<float>(coord.x), static_cast<float>(coord.y));
}

Vector2 Layout::GetGridCoord(Vector2 screenPos) const noexcept {
    return {
        (screenPos.x - m_gridStartX) / (Config::Grid::CELL_SIZE * m_scale),
        (screenPos.y - m_gridStartY) / (Config::Grid::CELL_SIZE * m_scale)
    };
}

float Layout::GetScaledCellSize() const noexcept {
    return static_cast<float>(Config::Grid::CELL_SIZE * m_scale);
}

float Layout::GetScaledValue(float value) const noexcept {
    return value * static_cast<float>(m_scale);
}
