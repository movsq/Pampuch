#include "Game.h"
#include "states/MenuScreens.h"
#include "states/EditorScreen.h"
#include "states/PlayScreen.h"
#include "states/GameOverScreen.h"

#include "core/Config.h"
#include "io/AssetPaths.h"
#include "io/MapIO.h"

#include <algorithm>

namespace GameHelpers {

void DrawCenteredTitle(const char* text, int yOffset, int fontSize, Color color) {
    const int textWidth = MeasureText(text, fontSize);
    DrawText(text, GetScreenWidth() / 2 - textWidth / 2, yOffset, fontSize, color);
}

} // namespace GameHelpers

Game::Game() {
    SetExitKey(KEY_NULL);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

    InitWindow(Config::Window::INITIAL_WIDTH, Config::Window::INITIAL_HEIGHT, Config::Window::TITLE);
    layout.Update(Config::Window::INITIAL_WIDTH, Config::Window::INITIAL_HEIGHT);

    LoadWindowIcon();

    audio.Init();

    const int monitorRefreshRate = GetMonitorRefreshRate(GetCurrentMonitor());
    SetTargetFPS(std::max(monitorRefreshRate, Config::Window::MIN_REFRESH_RATE));

    MapIO::EnsureFolder();
    postProcess.Init(AssetPaths::RETRO_SHADER);
}

Game::~Game() {
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        HandleGlobalInput();

        if (IsWindowResized()) {
            layout.Update(GetScreenWidth(), GetScreenHeight());
            postProcess.UpdateSize(GetScreenWidth(), GetScreenHeight());
        }

        const bool editorActive = (currentState == GameState::Editor ||
                                   currentState == GameState::EditorLevelSelect);
        layout.SetHudMode(editorActive ? Layout::HudMode::Sidebar : Layout::HudMode::TopBar);

        const float dt = GetFrameTime();
        Update(dt);
        Draw();
    }
}

void Game::LoadWindowIcon() {
    Image iconImage = LoadImage(AssetPaths::ICON_PATH);
    if (iconImage.data != nullptr) {
        SetWindowIcon(iconImage);
        UnloadImage(iconImage);
    } else {
        TraceLog(LOG_WARNING, "GAME: Failed to load window icon: %s", AssetPaths::ICON_PATH);
    }
}

void Game::HandleGlobalInput() {
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }
}

void Game::ResetLevelState() {
    waitingForStart = true;
    player.Reset(activeLevel.startPos, layout);

    activeGhosts.clear();
    int colorIdx = 0;
    for (const auto& spawn : activeLevel.ghostSpawns) {
        activeGhosts.emplace_back(spawn.x, spawn.y, colorIdx,
            Ghost::COLORS[colorIdx % Ghost::COLORS.size()], layout);
        ++colorIdx;
    }
}

void Game::Update(float dt) {
    switch (currentState) {
        case GameState::Play:
            PlayScreen::Update(*this, dt);
            break;
        case GameState::Paused:
            if (IsKeyPressed(KEY_SPACE)) {
                currentState = GameState::Play;
            }
            break;
        case GameState::Editor:
            editor.HandleInput(activeLevel, layout);
            break;
        default:
            break;
    }
}

void Game::Draw() {
    postProcess.BeginCapture();

    switch (currentState) {
        case GameState::MenuMain:
            MenuScreens::DrawMenuMain(*this);
            break;

        case GameState::MenuEditorMain:
            MenuScreens::DrawMenuEditorMain(*this);
            break;

        case GameState::MenuPlaySelect:
            MenuScreens::DrawPackSelect(*this, /*isEdit=*/false);
            break;

        case GameState::MenuEditSelect:
            MenuScreens::DrawPackSelect(*this, /*isEdit=*/true);
            break;

        case GameState::MenuCreateInfo:
            MenuScreens::DrawMenuCreateInfo(*this);
            break;

        case GameState::EditorLevelSelect:
            EditorScreen::DrawLevelSelect(*this);
            break;

        case GameState::Editor:
            EditorScreen::DrawEditorMode(*this);
            break;

        case GameState::Paused:
        case GameState::Play:
            PlayScreen::Draw(*this);
            break;

        case GameState::GameOver:
            GameOverScreen::Draw(*this);
            break;
    }

    postProcess.EndCapture();

    if (currentState == GameState::Play) {
        postProcess.playerPos.x = player.pixelPos.x / static_cast<float>(GetScreenWidth());
        postProcess.playerPos.y = player.pixelPos.y / static_cast<float>(GetScreenHeight());
        postProcess.powerMode = turboMode ? 1.0f : 0.0f;
    } else {
        const Vector2 mp = GetMousePosition();
        postProcess.playerPos.x = mp.x / static_cast<float>(GetScreenWidth());
        postProcess.playerPos.y = mp.y / static_cast<float>(GetScreenHeight());
        postProcess.powerMode = 0.0f;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    postProcess.Render();
    DrawCopyright();
    EndDrawing();
}

void Game::DrawCopyright() const {
    constexpr const char* copyrightText = "Copyright \xC2\xA9 2025 Vojtech Sedlacek (FixeD)";
    constexpr int copyrightFontSize = 8;
    const int textWidth = MeasureText(copyrightText, copyrightFontSize);
    DrawText(copyrightText,
        GetScreenWidth() - textWidth - Config::Grid::MARGIN,
        GetScreenHeight() - copyrightFontSize - Config::Grid::MARGIN + 13,
        copyrightFontSize,
        { DARKGRAY.r, DARKGRAY.g, DARKGRAY.b, 200 });
}
