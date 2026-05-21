#include "Level.h"
#include "render/Layout.h"
#include <array>
#include <queue>

Level::Level() {
    for (auto& col : tiles) col.fill(TileType::Floor);
}

bool Level::IsWalkable(int x, int y) const noexcept {
    if (!IsWithinBounds(x, y)) return false;
    return tiles[x][y] != TileType::Wall;
}

bool Level::IsWalkable(GridCoord coord) const noexcept {
    return IsWalkable(coord.x, coord.y);
}

bool Level::IsWithinBounds(int x, int y) const noexcept {
    return (x >= 0 && x < Config::Grid::COLS && y >= 0 && y < Config::Grid::ROWS);
}

bool Level::IsWithinBounds(GridCoord coord) const noexcept {
    return IsWithinBounds(coord.x, coord.y);
}

void Level::SetTile(int x, int y, TileType type) {
    if (IsWithinBounds(x, y)) tiles[x][y] = type;
}

void Level::SetTile(GridCoord coord, TileType type) {
    SetTile(coord.x, coord.y, type);
}

TileType Level::GetTile(int x, int y) const noexcept {
    if (!IsWithinBounds(x, y)) return TileType::Wall;
    return tiles[x][y];
}

TileType Level::GetTile(GridCoord coord) const noexcept {
    return GetTile(coord.x, coord.y);
}

void Level::ClearPoints() {
    for (auto& col : tiles) {
        for (auto& tile : col) {
            if (tile == TileType::Point) tile = TileType::Floor;
        }
    }
}

int Level::CountPoints() const noexcept {
    int count = 0;
    for (const auto& col : tiles) {
        for (const auto& tile : col) {
            if (tile == TileType::Point) ++count;
        }
    }
    return count;
}

void Level::PerformFloodFill(int fillStartX, int fillStartY) {
    if (!IsWithinBounds(fillStartX, fillStartY)) return;
    if (tiles[fillStartX][fillStartY] != TileType::Floor) return;

    std::queue<GridCoord> q;
    q.push({ fillStartX, fillStartY });
    tiles[fillStartX][fillStartY] = TileType::Point;

    static constexpr std::array<GridCoord, 4> DIRECTIONS = {{
        { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 }
    }};

    while (!q.empty()) {
        const auto [cx, cy] = q.front();
        q.pop();

        for (const auto& dir : DIRECTIONS) {
            const int nx = cx + dir.x;
            const int ny = cy + dir.y;
            if (IsWithinBounds(nx, ny) && tiles[nx][ny] == TileType::Floor) {
                tiles[nx][ny] = TileType::Point;
                q.push({ nx, ny });
            }
        }
    }
}

void Level::Draw(const Layout& layout, bool isEditorMode) const {
    const float scaledCell = layout.GetScaledCellSize();
    const Color gridColor = { 255, 255, 255, Config::Render::GRID_LINE_ALPHA };
    const int scale = layout.Scale();

    for (int x = 0; x < Config::Grid::COLS; ++x) {
        for (int y = 0; y < Config::Grid::ROWS; ++y) {
            const Vector2 pos = layout.GetScreenPos(static_cast<float>(x), static_cast<float>(y));
            const TileType type = tiles[x][y];

            if (isEditorMode) {
                DrawRectangleLinesEx({ pos.x, pos.y, scaledCell, scaledCell },
                    Config::Render::GRID_LINE_THICKNESS, gridColor);
            }

            if (type == TileType::Wall) {
                DrawRectangle(static_cast<int>(pos.x), static_cast<int>(pos.y),
                    static_cast<int>(scaledCell), static_cast<int>(scaledCell), DARKGRAY);

                const float thick = Config::Render::WALL_OUTLINE_THICKNESS * scale;
                const Color outline = LIGHTGRAY;

                if (y == 0 || tiles[x][y - 1] != TileType::Wall)
                    DrawRectangle(static_cast<int>(pos.x), static_cast<int>(pos.y),
                        static_cast<int>(scaledCell), static_cast<int>(thick), outline);
                if (y == Config::Grid::ROWS - 1 || tiles[x][y + 1] != TileType::Wall)
                    DrawRectangle(static_cast<int>(pos.x), static_cast<int>(pos.y + scaledCell - thick),
                        static_cast<int>(scaledCell), static_cast<int>(thick), outline);
                if (x == 0 || tiles[x - 1][y] != TileType::Wall)
                    DrawRectangle(static_cast<int>(pos.x), static_cast<int>(pos.y),
                        static_cast<int>(thick), static_cast<int>(scaledCell), outline);
                if (x == Config::Grid::COLS - 1 || tiles[x + 1][y] != TileType::Wall)
                    DrawRectangle(static_cast<int>(pos.x + scaledCell - thick), static_cast<int>(pos.y),
                        static_cast<int>(thick), static_cast<int>(scaledCell), outline);
            } else if (type == TileType::Point) {
                DrawCircle(
                    static_cast<int>(pos.x + scaledCell / 2),
                    static_cast<int>(pos.y + scaledCell / 2),
                    Config::Render::POINT_RADIUS * scale, GOLD);
            }
        }
    }

    if (isEditorMode) {
        const Vector2 startScreen = layout.GetScreenPos(startPos);
        const float thickness = Config::Render::EDITOR_START_MARKER_THICKNESS * scale;
        DrawRectangleLinesEx({ startScreen.x, startScreen.y, scaledCell, scaledCell }, thickness, GREEN);
        DrawText("S",
            static_cast<int>(startScreen.x + 5),
            static_cast<int>(startScreen.y + 5),
            Config::Render::EDITOR_START_FONT_SIZE * scale, GREEN);
    }
}
