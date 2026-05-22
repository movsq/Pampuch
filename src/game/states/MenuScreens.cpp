#include "MenuScreens.h"
#include "game/Game.h"
#include "core/Config.h"
#include "io/MapIO.h"
#include "ui/Widgets.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace MenuScreens {

static void DrawMenuBackground() {
    ClearBackground(Config::UI::DARK_BG);

    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    // Vignette: subtle darker wash near the edges.
    DrawRectangleGradientV(0, 0, screenW, screenH / 3,
        Fade(BLACK, 0.35f), Fade(BLACK, 0.0f));
    DrawRectangleGradientV(0, screenH - screenH / 3, screenW, screenH / 3,
        Fade(BLACK, 0.0f), Fade(BLACK, 0.4f));

    // Pellet dot grid backdrop.
    constexpr int spacing = 48;
    const Color dotColor = { 255, 255, 255, 12 };
    for (int y = spacing / 2; y < screenH; y += spacing) {
        for (int x = spacing / 2; x < screenW; x += spacing) {
            DrawCircle(x, y, 2.0f, dotColor);
        }
    }
}

void DrawMenuMain(Game& game) {
    DrawMenuBackground();

    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    // Title with drop shadow.
    constexpr int titleSize = 80;
    const char* title = "PAMPUCH";
    const int titleW = MeasureText(title, titleSize);
    const int titleX = screenW / 2 - titleW / 2;
    const int titleY = static_cast<int>(screenH * 0.14f);
    DrawText(title, titleX + 4, titleY + 4, titleSize, Fade(BLACK, 0.65f));
    DrawText(title, titleX, titleY, titleSize, GOLD);

    // Gold underline beneath the title.
    const int underlineW = 96;
    const int underlineY = titleY + titleSize + 6;
    DrawRectangle(screenW / 2 - underlineW / 2, underlineY, underlineW, 3, Fade(GOLD, 0.85f));

    // Subtitle.
    constexpr int subtitleSize = 20;
    const char* subtitle = "a tribute to Zoner's classic";
    const int subW = MeasureText(subtitle, subtitleSize);
    DrawText(subtitle, screenW / 2 - subW / 2, underlineY + 14, subtitleSize, RAYWHITE);

    // Main menu buttons.
    const bool largeScreen = screenH > Config::UI::MENU_BUTTON_SCALE_THRESHOLD;
    const float scaleFactor = largeScreen ? Config::UI::MENU_BUTTON_SCALE_FACTOR : 1.0f;
    const float btnW = 260 * scaleFactor;
    const float btnH = 52 * scaleFactor;
    const float cx = screenW / 2.0f - btnW / 2.0f;
    const float startY = static_cast<float>(underlineY) + 60.0f * scaleFactor;
    const float gap = 16.0f * scaleFactor + btnH;

    if (UI::Button({ cx, startY, btnW, btnH }, "PLAY", game.layout)) {
        game.currentState = GameState::MenuPlaySelect;
    }
    if (UI::Button({ cx, startY + gap, btnW, btnH }, "EDITOR", game.layout)) {
        game.currentState = GameState::MenuEditorMain;
    }
    if (UI::Button({ cx, startY + gap * 2, btnW, btnH }, "EXIT", game.layout)) {
        CloseWindow();
    }

    // Bottom toggle row, centered.
    int fontSize = Config::UI::BASE_FONT_SIZE * game.layout.Scale();
    if (fontSize > Config::UI::MAX_FONT_SIZE) fontSize = Config::UI::MAX_FONT_SIZE;

    const bool isFullscreen = IsWindowFullscreen();
    std::string fullscreenText = isFullscreen ? "FULLSCREEN: ON" : "FULLSCREEN: OFF";
    std::string turboText = game.turboMode ? "TURBO: ON" : "TURBO: OFF";
    std::string infText = game.infiniteLives ? "INF LIVES: ON" : "INF LIVES: OFF";

    const float paddingX = 14.0f * game.layout.Scale();
    const float toggleBtnH = Config::UI::BUTTON_HEIGHT * game.layout.Scale();
    const float fullscreenBtnW = static_cast<float>(MeasureText(fullscreenText.c_str(), fontSize)) + paddingX * 2.0f;
    const float turboBtnW = static_cast<float>(MeasureText(turboText.c_str(), fontSize)) + paddingX * 2.0f;
    const float infBtnW = static_cast<float>(MeasureText(infText.c_str(), fontSize)) + paddingX * 2.0f;
    const float toggleGap = 14.0f;
    const float totalToggleW = fullscreenBtnW + turboBtnW + infBtnW + toggleGap * 2.0f;
    const float toggleStartX = (screenW - totalToggleW) / 2.0f;
    const float bottomY = screenH - toggleBtnH - 24.0f;

    if (UI::Button({ toggleStartX, bottomY, fullscreenBtnW, toggleBtnH }, fullscreenText, game.layout)) {
        ToggleFullscreen();
    }

    if (UI::Button({ toggleStartX + fullscreenBtnW + toggleGap, bottomY, turboBtnW, toggleBtnH }, turboText, game.layout)) {
        game.turboMode = !game.turboMode;
    }

    if (UI::Button({ toggleStartX + fullscreenBtnW + turboBtnW + toggleGap * 2.0f, bottomY, infBtnW, toggleBtnH }, infText, game.layout)) {
        game.infiniteLives = !game.infiniteLives;
    }
}

void DrawMenuEditorMain(Game& game) {
    ClearBackground(Config::UI::DARK_BG);
    GameHelpers::DrawCenteredTitle("EDITOR MENU", 80, 40, LIGHTGRAY);

    const float cx = GetScreenWidth() / 2.0f - 120;
    if (UI::Button({ cx, 200, 240, 50 }, "EDIT EXISTING", game.layout)) {
        game.currentState = GameState::MenuEditSelect;
    }
    if (UI::Button({ cx, 270, 240, 50 }, "CREATE NEW", game.layout)) {
        game.inputNameBuffer.clear();
        game.inputAuthorBuffer.clear();
        game.currentState = GameState::MenuCreateInfo;
    }
    if (UI::Button({ cx, 340, 240, 50 }, "BACK", game.layout)) {
        game.currentState = GameState::MenuMain;
    }
}

void DrawPackSelect(Game& game, bool isEdit) {
    ClearBackground(Config::UI::DARK_BG);
    DrawText(isEdit ? "SELECT TO EDIT" : "SELECT TO PLAY", 50, 50, 30, WHITE);

    const auto files = MapIO::GetAvailablePacks();
    int startY = 100;

    if (files.empty()) {
        DrawText("No maps found in ./maps/", 50, 100, 20, GRAY);
    }

    for (const auto& file : files) {
        const std::string filename = fs::path(file).stem().string();
        if (UI::Button({ 50, static_cast<float>(startY), 400, 40 }, filename, game.layout)) {
            if (MapIO::LoadPack(file, game.currentPack)) {
                if (isEdit) {
                    game.currentState = GameState::EditorLevelSelect;
                } else if (!game.currentPack.levels.empty()) {
                    game.currentLevelIndex = 0;
                    game.activeLevel = game.currentPack.levels[0];
                    game.player.lives = Config::Gameplay::STARTING_LIVES;
                    game.player.score = 0;
                    game.ResetLevelState();
                    game.currentState = GameState::Play;
                }
            }
        }
        startY += 50;
    }

    if (UI::Button({ 50, static_cast<float>(GetScreenHeight() - 60), 100, 40 }, "BACK", game.layout)) {
        game.currentState = GameState::MenuEditorMain;
    }
}

void DrawMenuCreateInfo(Game& game) {
    ClearBackground(Config::UI::SIDEBAR_BG);
    const int cx = GetScreenWidth() / 2 - 150;
    int cy = 150;

    DrawText("New Pack Name:", cx, cy, 20, LIGHTGRAY);
    const Rectangle nameR = { static_cast<float>(cx), static_cast<float>(cy + 25), 300, 30 };
    DrawRectangleRec(nameR, game.inputFocus == InputFocus::CreatePackName ? WHITE : LIGHTGRAY);
    DrawText(game.inputNameBuffer.c_str(), static_cast<int>(nameR.x + 5), static_cast<int>(nameR.y + 5), 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), nameR) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game.inputFocus = InputFocus::CreatePackName;
    }

    cy += 80;
    DrawText("Author:", cx, cy, 20, LIGHTGRAY);
    const Rectangle authR = { static_cast<float>(cx), static_cast<float>(cy + 25), 300, 30 };
    DrawRectangleRec(authR, game.inputFocus == InputFocus::CreateAuthor ? WHITE : LIGHTGRAY);
    DrawText(game.inputAuthorBuffer.c_str(), static_cast<int>(authR.x + 5), static_cast<int>(authR.y + 5), 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), authR) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game.inputFocus = InputFocus::CreateAuthor;
    }

    if (game.inputFocus == InputFocus::CreatePackName) UI::HandleTextInput(game.inputNameBuffer, Config::UI::MAX_NAME_LENGTH);
    if (game.inputFocus == InputFocus::CreateAuthor)   UI::HandleTextInput(game.inputAuthorBuffer, Config::UI::MAX_NAME_LENGTH);

    if (!game.inputNameBuffer.empty() &&
        UI::Button({ static_cast<float>(cx), static_cast<float>(cy + 80), 300, 50 }, "CREATE", game.layout))
    {
        game.currentPack = MapPack();
        game.currentPack.name = game.inputNameBuffer;
        game.currentPack.author = game.inputAuthorBuffer;
        game.currentState = GameState::EditorLevelSelect;
        game.inputFocus = InputFocus::None;
    }

    if (UI::Button({ 20, 20, 80, 40 }, "BACK", game.layout)) {
        game.currentState = GameState::MenuEditorMain;
    }
}

} // namespace MenuScreens
