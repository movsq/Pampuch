#pragma once

#include "TileType.h"
#include "core/GridCoord.h"
#include "core/Config.h"
#include <array>
#include <vector>

class Layout;

class Level {
public:
    std::array<std::array<TileType, Config::Grid::ROWS>, Config::Grid::COLS> tiles{};
    GridCoord startPos{ 0, 0 };
    std::vector<GridCoord> ghostSpawns;

    Level();

    [[nodiscard]] bool IsWalkable(int x, int y) const noexcept;
    [[nodiscard]] bool IsWalkable(GridCoord coord) const noexcept;
    [[nodiscard]] bool IsWithinBounds(int x, int y) const noexcept;
    [[nodiscard]] bool IsWithinBounds(GridCoord coord) const noexcept;

    void SetTile(int x, int y, TileType type);
    void SetTile(GridCoord coord, TileType type);
    [[nodiscard]] TileType GetTile(int x, int y) const noexcept;
    [[nodiscard]] TileType GetTile(GridCoord coord) const noexcept;

    void ClearPoints();
    [[nodiscard]] int CountPoints() const noexcept;
    void PerformFloodFill(int fillStartX, int fillStartY);

    void Draw(const Layout& layout, bool isEditorMode) const;
};
