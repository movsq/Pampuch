#include "EditorScreen.h"
#include "game/Game.h"
#include "core/Config.h"
#include "io/MapIO.h"
#include "ui/Widgets.h"
#include "entities/Ghost.h"

namespace EditorScreen {

void DrawLevelSelect(Game& game) {
    ClearBackground(Config::UI::SIDEBAR_BG);
    int startY = 20;

    auto DrawInput = [&](const char* label, std::string& buf, InputFocus focusID) {
        DrawText(label, 20, startY + 5, 20, GOLD);
        const Rectangle rect = { 140, static_cast<float>(startY), 300, 30 };
        const bool isFocused = (game.inputFocus == focusID);

        if (CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game.inputFocus = focusID;
        }

        DrawRectangleRec(rect, isFocused ? WHITE : LIGHTGRAY);
        DrawRectangleLinesEx(rect, 1, isFocused ? GREEN : DARKGRAY);
        DrawText(buf.c_str(), static_cast<int>(rect.x + 5), static_cast<int>(rect.y + 5), 20, BLACK);

        if (isFocused) {
            UI::HandleTextInput(buf, Config::UI::MAX_INPUT_LENGTH);
        }
        startY += 40;
    };

    DrawInput("Pack Name:", game.currentPack.name, InputFocus::EditPackName);
    DrawInput("Author:",    game.currentPack.author, InputFocus::EditAuthor);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() > 450) {
        game.inputFocus = InputFocus::None;
    }

    int listStartY = startY + 50;
    int listX = 50;
    for (size_t i = 0; i < game.currentPack.levels.size(); ++i) {
        if (UI::Button({ static_cast<float>(listX), static_cast<float>(listStartY), 200, 40 },
            TextFormat("Level %zu", i + 1), game.layout))
        {
            game.currentLevelIndex = static_cast<int>(i);
            game.activeLevel = game.currentPack.levels[i];
            game.currentState = GameState::Editor;
            game.inputFocus = InputFocus::None;
        }
        listStartY += 50;
        if (listStartY > GetScreenHeight() - 200) {
            listStartY = startY + 50;
            listX += 220;
        }
    }

    if (UI::Button({ static_cast<float>(listX), static_cast<float>(listStartY), 200, 40 }, "[+] NEW LEVEL", game.layout)) {
        game.currentLevelIndex = static_cast<int>(game.currentPack.levels.size());
        game.activeLevel = Level();
        game.currentState = GameState::Editor;
        game.inputFocus = InputFocus::None;
    }

    if (UI::Button({ 20, static_cast<float>(GetScreenHeight() - 60), 200, 40 }, "MENU", game.layout)) {
        game.currentState = GameState::MenuMain;
        game.inputFocus = InputFocus::None;
    }
}

void DrawEditorMode(Game& game) {
    ClearBackground(Config::UI::DARKER_BG);
    game.activeLevel.Draw(game.layout, /*isEditorMode=*/true);

    int gIdx = 0;
    for (const auto& spawn : game.activeLevel.ghostSpawns) {
        const Vector2 pos = game.layout.GetScreenPos(spawn);
        Ghost temp(0, 0, gIdx, Fade(Ghost::COLORS[gIdx % Ghost::COLORS.size()], 0.7f), game.layout);
        temp.pixelPos = pos;
        temp.Draw(GetMousePosition(), game.layout);
        ++gIdx;
    }

    game.editor.DrawSidebar(game.currentPack.name, game.currentLevelIndex,
        static_cast<int>(game.activeLevel.ghostSpawns.size()), game.layout);

    const float btnW = game.layout.SidebarWidth() - 20;
    if (UI::Button({ 10, static_cast<float>(GetScreenHeight() - 110), btnW, 40 },
        "SAVE LEVEL & BACK", game.layout))
    {
        if (game.currentLevelIndex < static_cast<int>(game.currentPack.levels.size())) {
            game.currentPack.levels[game.currentLevelIndex] = game.activeLevel;
        } else {
            game.currentPack.levels.push_back(game.activeLevel);
        }
        MapIO::SavePack(game.currentPack);
        game.currentState = GameState::EditorLevelSelect;
    }

    if (UI::Button({ 10, static_cast<float>(GetScreenHeight() - 60), btnW, 40 },
        "DISCARD & BACK", game.layout))
    {
        game.currentState = GameState::EditorLevelSelect;
    }
}

} // namespace EditorScreen
