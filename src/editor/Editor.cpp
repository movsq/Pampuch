#include "Editor.h"
#include "core/Config.h"
#include "entities/Ghost.h"
#include "render/Layout.h"
#include "world/Level.h"
#include <algorithm>

const std::vector<ToolDefinition>& Editor::GetTools() {
    static const std::vector<ToolDefinition> tools = {
        { EditorTool::Wall,        "Wall",       "1", "Blocks movement.", DARKGRAY },
        { EditorTool::Floor,       "Floor",      "2", "Walkable (Eraser).", LIGHTGRAY },
        { EditorTool::Point,       "Point",      "3", "Objective.", GOLD },
        { EditorTool::Fill,        "Fill pts.",  "4", "Fill area.", PURPLE },
        { EditorTool::ClearPoints, "Clear pts.", "5", "Remove points.", RED },
        { EditorTool::Start,       "Start",      "6", "Spawn point.", GREEN },
        { EditorTool::Ghost,       "Ghost",      "7", "Enemy Spawn (Max 6).", RED },
    };
    return tools;
}

void Editor::HandleInput(Level& level, const Layout& layout) {
    HandleToolSelection();

    const Vector2 mousePos = GetMousePosition();
    const float gridWidth = static_cast<float>(Config::Grid::COLS * Config::Grid::CELL_SIZE * layout.Scale());

    if (mousePos.x >= layout.GridStartX() && mousePos.x < layout.GridStartX() + gridWidth) {
        const Vector2 gridCoords = layout.GetGridCoord(mousePos);
        const int mx = static_cast<int>(gridCoords.x);
        const int my = static_cast<int>(gridCoords.y);

        if (level.IsWithinBounds(mx, my)) {
            const Vector2 screenPos = layout.GetScreenPos(static_cast<float>(mx), static_cast<float>(my));
            const float cellSize = layout.GetScaledCellSize();
            DrawRectangleLines(
                static_cast<int>(screenPos.x),
                static_cast<int>(screenPos.y),
                static_cast<int>(cellSize),
                static_cast<int>(cellSize), GREEN);

            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                HandleErase(level, mx, my);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ApplyTool(mx, my, level);
            } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (currentTool == EditorTool::Wall ||
                    currentTool == EditorTool::Floor ||
                    currentTool == EditorTool::Point) {
                    ApplyTool(mx, my, level);
                }
            }
        }
    }
}

void Editor::DrawSidebar(const std::string& packName, int levelIdx, int ghostCount, const Layout& layout) {
    const int scale = layout.Scale();
    const float sidebarWidth = layout.SidebarWidth();

    DrawRectangle(0, 0, static_cast<int>(sidebarWidth), GetScreenHeight(), Config::UI::SIDEBAR_BG);
    DrawLine(static_cast<int>(sidebarWidth), 0,
        static_cast<int>(sidebarWidth), GetScreenHeight(), GRAY);

    int fontSize = Config::UI::BASE_FONT_SIZE * scale;
    int smFont = Config::UI::SMALL_FONT_SIZE * scale;
    if (fontSize > Config::UI::MAX_SIDEBAR_FONT_SIZE) fontSize = Config::UI::MAX_SIDEBAR_FONT_SIZE;

    DrawText("CREATING MAP PACK:", 10, 10, smFont, GRAY);
    DrawText(packName.c_str(), 10, 10 + smFont + 5, fontSize, GOLD);
    DrawText(TextFormat("Level: %d", levelIdx + 1), 10, 10 + smFont + 5 + fontSize + 5, fontSize, WHITE);
    DrawText(TextFormat("Ghosts: %d/%d", ghostCount, Config::Gameplay::MAX_GHOSTS),
        10, 10 + smFont + 5 + fontSize * 2 + 5, smFont, RED);

    float startY = static_cast<float>(10 + smFont + 5 + fontSize * 2 + 5 + smFont + 20);
    const float btnHeight = Config::UI::BUTTON_HEIGHT * scale;
    const float gap = Config::UI::BUTTON_GAP * scale;

    for (const auto& tool : GetTools()) {
        const Rectangle btnRect = { 10.0f, startY, sidebarWidth - 20, btnHeight };
        const bool isSelected = (currentTool == tool.type);
        const bool isHovered = CheckCollisionPointRec(GetMousePosition(), btnRect);

        const Color bgColor = isSelected ? Fade(GREEN, 0.5f)
            : (isHovered ? Fade(WHITE, 0.2f) : Fade(BLACK, 0.5f));
        DrawRectangleRec(btnRect, bgColor);
        DrawRectangleLinesEx(btnRect, 1, isSelected ? GREEN : LIGHTGRAY);

        const Rectangle iconRect = {
            btnRect.x + Config::Render::TOOL_ICON_PADDING,
            btnRect.y + Config::Render::TOOL_ICON_OFFSET_Y * scale,
            Config::Render::TOOL_ICON_SIZE * scale,
            Config::Render::TOOL_ICON_SIZE * scale
        };
        DrawToolIcon(tool, iconRect, layout);

        DrawText(tool.name.c_str(),
            static_cast<int>(iconRect.x + iconRect.width + 10),
            static_cast<int>(btnRect.y + (btnRect.height - fontSize) / 2.0f),
            fontSize, WHITE);

        const std::string keyText = "[" + tool.key + "]";
        const int keyWidth = MeasureText(keyText.c_str(), smFont);
        DrawText(keyText.c_str(),
            static_cast<int>(btnRect.x + btnRect.width - keyWidth - 8),
            static_cast<int>(btnRect.y + (btnRect.height - smFont) / 2.0f),
            smFont, LIGHTGRAY);

        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            currentTool = tool.type;
        }
        startY += btnHeight + gap;
    }
}

void Editor::HandleToolSelection() {
    if (IsKeyPressed(KEY_ONE))   currentTool = EditorTool::Wall;
    if (IsKeyPressed(KEY_TWO))   currentTool = EditorTool::Floor;
    if (IsKeyPressed(KEY_THREE)) currentTool = EditorTool::Point;
    if (IsKeyPressed(KEY_FOUR))  currentTool = EditorTool::Fill;
    if (IsKeyPressed(KEY_FIVE))  currentTool = EditorTool::ClearPoints;
    if (IsKeyPressed(KEY_SIX))   currentTool = EditorTool::Start;
    if (IsKeyPressed(KEY_SEVEN)) currentTool = EditorTool::Ghost;
}

void Editor::HandleErase(Level& level, int mx, int my) {
    level.SetTile(mx, my, TileType::Floor);
    RemoveGhostSpawnAt(level, mx, my);
    if (level.startPos.x == mx && level.startPos.y == my) {
        level.startPos = { 0, 0 };
    }
}

void Editor::RemoveGhostSpawnAt(Level& level, int x, int y) {
    auto& ghosts = level.ghostSpawns;
    ghosts.erase(std::remove_if(ghosts.begin(), ghosts.end(),
        [x, y](const GridCoord& p) { return p.x == x && p.y == y; }), ghosts.end());
}

void Editor::ApplyTool(int mx, int my, Level& level) {
    switch (currentTool) {
        case EditorTool::Fill:
            level.PerformFloodFill(mx, my);
            break;

        case EditorTool::ClearPoints:
            level.ClearPoints();
            break;

        case EditorTool::Ghost:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                auto it = std::find_if(level.ghostSpawns.begin(), level.ghostSpawns.end(),
                    [mx, my](const GridCoord& p) { return p.x == mx && p.y == my; });

                if (it != level.ghostSpawns.end()) {
                    level.ghostSpawns.erase(it);
                } else if (level.ghostSpawns.size() < static_cast<size_t>(Config::Gameplay::MAX_GHOSTS)) {
                    if (mx != level.startPos.x || my != level.startPos.y) {
                        if (level.GetTile(mx, my) == TileType::Wall) {
                            level.SetTile(mx, my, TileType::Floor);
                        }
                        level.ghostSpawns.push_back({ mx, my });
                    }
                }
            }
            break;

        case EditorTool::Start:
            level.startPos = { mx, my };
            level.SetTile(mx, my, TileType::Floor);
            RemoveGhostSpawnAt(level, mx, my);
            break;

        case EditorTool::Wall:
            level.SetTile(mx, my, TileType::Wall);
            RemoveGhostSpawnAt(level, mx, my);
            break;

        case EditorTool::Floor:
            level.SetTile(mx, my, TileType::Floor);
            break;

        case EditorTool::Point:
            if (mx != level.startPos.x || my != level.startPos.y) {
                level.SetTile(mx, my, TileType::Point);
            }
            break;
    }
}

void Editor::DrawToolIcon(const ToolDefinition& tool, Rectangle iconRect, const Layout& layout) const {
    const float s = layout.GetScaledValue(1.0f);

    switch (tool.type) {
        case EditorTool::Point:
            DrawCircle(
                static_cast<int>(iconRect.x + iconRect.width / 2),
                static_cast<int>(iconRect.y + iconRect.height / 2),
                Config::Render::TOOL_POINT_RADIUS * s, tool.color);
            break;

        case EditorTool::Start:
            DrawRectangleLinesEx(iconRect, 2, tool.color);
            DrawText("S",
                static_cast<int>(iconRect.x + 10),
                static_cast<int>(iconRect.y + 5),
                static_cast<int>(20 * s), tool.color);
            break;

        case EditorTool::Ghost: {
            Ghost temp(0, 0, 0, tool.color, layout);
            temp.pixelPos = { iconRect.x - 1, iconRect.y - 1 };
            temp.Draw({ iconRect.x + 50, iconRect.y + 50 }, layout);
            break;
        }

        case EditorTool::Fill:
            DrawRectangleRec(iconRect, tool.color);
            DrawText("F",
                static_cast<int>(iconRect.x + 10),
                static_cast<int>(iconRect.y + 5),
                static_cast<int>(20 * s), BLACK);
            break;

        case EditorTool::ClearPoints:
            DrawRectangleLinesEx(iconRect, 2, tool.color);
            DrawLine(
                static_cast<int>(iconRect.x + 5),
                static_cast<int>(iconRect.y + 5),
                static_cast<int>(iconRect.x + iconRect.width - 5),
                static_cast<int>(iconRect.y + iconRect.height - 5), tool.color);
            DrawLine(
                static_cast<int>(iconRect.x + iconRect.width - 5),
                static_cast<int>(iconRect.y + 5),
                static_cast<int>(iconRect.x + 5),
                static_cast<int>(iconRect.y + iconRect.height - 5), tool.color);
            break;

        default:
            DrawRectangleRec(iconRect, tool.color);
            break;
    }
}
