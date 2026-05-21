#include "PlayScreen.h"
#include "game/Game.h"
#include "core/Config.h"
#include "ui/Widgets.h"

#include <algorithm>
#include <string>

namespace PlayScreen {

void Update(Game& game, float dt) {
    if (IsKeyPressed(KEY_SPACE)) {
        game.currentState = GameState::Paused;
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.currentState = GameState::MenuMain;
        return;
    }
    game.player.Update(dt, game.activeLevel, game.layout, game.waitingForStart, game.turboMode);

    if (!game.waitingForStart) {
        for (auto& ghost : game.activeGhosts) {
            ghost.Update(dt, game.activeLevel, game.player, game.activeGhosts, game.layout, game.turboMode);

            const float colRad = Config::Physics::COLLISION_RADIUS * game.layout.Scale();
            const float dx = ghost.pixelPos.x - game.player.pixelPos.x;
            const float dy = ghost.pixelPos.y - game.player.pixelPos.y;

            if ((dx * dx + dy * dy) < colRad * colRad) {
                --game.player.lives;

                if (game.player.lives > 0) {
                    game.ResetLevelState();
                } else {
                    game.currentState = GameState::GameOver;
                }
                return;
            }
        }
    }

    if (game.activeLevel.CountPoints() == 0) {
        ++game.currentLevelIndex;
        if (game.currentLevelIndex < static_cast<int>(game.currentPack.levels.size())) {
            game.activeLevel = game.currentPack.levels[game.currentLevelIndex];
            game.ResetLevelState();
        } else {
            game.currentState = GameState::GameOver;
        }
    }
}

static void DrawLifeIcon(float cx, float cy, float radius, Color col) {
    // Mirrors Player::Draw — face oriented right, idle mouth angle, eye dot at top-front.
    constexpr float mouthAngle = Config::Render::PLAYER_MOUTH_OPEN_IDLE;
    DrawCircleSector({ cx, cy }, radius,
        mouthAngle, 360.0f - mouthAngle,
        Config::Render::PLAYER_MOUTH_SEGMENTS, col);
    const float majorOff = radius * 0.55f;
    const float minorOff = radius * 0.25f;
    DrawCircleV({ cx + minorOff, cy - majorOff }, radius * 0.18f, BLACK);
}

static void DrawHudDivider(float x, float barH) {
    DrawLineEx({ x, barH * 0.22f }, { x, barH * 0.78f }, 1.0f, Fade(WHITE, 0.18f));
}

void Draw(Game& game) {
    ClearBackground(Config::UI::DARKER_BG);

    const int scale = game.layout.Scale();
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());
    const float barH = game.layout.HudBarHeight();

    int fontSize = Config::UI::BASE_FONT_SIZE * scale;
    if (fontSize > Config::UI::MAX_SIDEBAR_FONT_SIZE) fontSize = Config::UI::MAX_SIDEBAR_FONT_SIZE;
    int smFont = Config::UI::SMALL_FONT_SIZE * scale;
    if (smFont < 10) smFont = 10;
    if (smFont > 18) smFont = 18;

    // Bar background + bottom accent
    DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(barH), Config::UI::DARK_BG);
    DrawLineEx({ 0, barH }, { screenW, barH }, 2.0f, Fade(GOLD, 0.55f));
    DrawLineEx({ 0, barH - 2.0f }, { screenW, barH - 2.0f }, 1.0f, Fade(GOLD, 0.18f));

    const float pad = 16.0f * scale;
    const float labelY = barH * 0.18f;
    const float valueY = barH * 0.46f;
    float cursorX = pad;

    // SCORE block
    DrawText("SCORE", static_cast<int>(cursorX), static_cast<int>(labelY), smFont, GOLD);
    const char* scoreStr = TextFormat("%06d", game.player.score);
    DrawText(scoreStr, static_cast<int>(cursorX), static_cast<int>(valueY), fontSize, WHITE);
    cursorX += static_cast<float>(MeasureText("000000", fontSize)) + pad;

    DrawHudDivider(cursorX, barH);
    cursorX += pad;

    // PACK / LEVEL block
    DrawText(game.currentPack.name.c_str(),
        static_cast<int>(cursorX), static_cast<int>(labelY), smFont, GOLD);
    const std::string levelStr = "Level " + std::to_string(game.currentLevelIndex + 1)
        + " / " + std::to_string(game.currentPack.levels.size());
    DrawText(levelStr.c_str(),
        static_cast<int>(cursorX), static_cast<int>(valueY), fontSize, LIGHTGRAY);
    const float packW = static_cast<float>(std::max(
        MeasureText(game.currentPack.name.c_str(), smFont),
        MeasureText(levelStr.c_str(), fontSize)));
    cursorX += packW + pad;

    DrawHudDivider(cursorX, barH);
    cursorX += pad;

    // LIVES (Pampuch icons)
    DrawText("LIVES", static_cast<int>(cursorX), static_cast<int>(labelY), smFont, GOLD);
    const Color lifeCol = game.player.lives > Config::Gameplay::LIFE_WARNING_THRESHOLD ? YELLOW : RED;
    const float lifeIconRadius = 9.0f * scale;
    const float lifeIconGap = 4.0f * scale;
    const float lifeCenterY = valueY + fontSize * 0.5f;
    const int livesDrawn = std::min(game.player.lives, 8);
    float lifeX = cursorX + lifeIconRadius;
    for (int i = 0; i < livesDrawn; ++i) {
        DrawLifeIcon(lifeX, lifeCenterY, lifeIconRadius, lifeCol);
        lifeX += lifeIconRadius * 2.0f + lifeIconGap;
    }
    if (game.player.lives > 8) {
        DrawText(TextFormat("+%d", game.player.lives - 8),
            static_cast<int>(lifeX),
            static_cast<int>(lifeCenterY - fontSize * 0.5f),
            fontSize, lifeCol);
        lifeX += static_cast<float>(MeasureText("+99", fontSize));
    }

    // QUIT (right edge) — ESC hint inlined into the label so it doesn't clip the bar.
    const char* quitLabel = "MENU [ESC]";
    const float quitBtnW = std::max(110.0f * scale, static_cast<float>(MeasureText(quitLabel, fontSize)) + pad);
    const float quitBtnH = barH * 0.6f;
    const float quitY = (barH - quitBtnH) / 2.0f;
    const Rectangle quitR = { screenW - pad - quitBtnW, quitY, quitBtnW, quitBtnH };
    if (UI::Button(quitR, quitLabel, game.layout)) {
        game.currentState = GameState::MenuMain;
    }

    // TURBO badge (right-aligned, left of quit)
    if (game.turboMode) {
        const float pillH = barH * 0.42f;
        const float pillTextW = static_cast<float>(MeasureText("TURBO", fontSize));
        const float pillW = pillTextW + pad;
        const Rectangle pill = { quitR.x - pad - pillW, (barH - pillH) / 2.0f, pillW, pillH };
        DrawRectangleRounded(pill, 1.0f, 8, Fade(RED, 0.75f));
        DrawRectangleRoundedLinesEx(pill, 1.0f, 8, 1.5f, Fade(WHITE, 0.55f));
        DrawText("TURBO",
            static_cast<int>(pill.x + (pill.width - pillTextW) / 2.0f),
            static_cast<int>(pill.y + (pill.height - fontSize) / 2.0f),
            fontSize, WHITE);
    }

    // Playfield
    game.activeLevel.Draw(game.layout, /*isEditorMode=*/false);

    const float gridW = static_cast<float>(Config::Grid::COLS * Config::Grid::CELL_SIZE * scale);
    const float gridH = static_cast<float>(Config::Grid::ROWS * Config::Grid::CELL_SIZE * scale);
    const float borderOffset = 3.0f * scale;
    const Rectangle playfield = {
        game.layout.GridStartX() - borderOffset,
        game.layout.GridStartY() - borderOffset,
        gridW + borderOffset * 2,
        gridH + borderOffset * 2
    };
    DrawRectangleLinesEx(playfield, 1.5f * scale, Fade(WHITE, 0.22f));

    game.player.Draw(game.layout);
    for (const auto& ghost : game.activeGhosts) {
        ghost.Draw(game.player.pixelPos, game.layout);
    }

    if (game.waitingForStart) {
        const char* txt = "GET READY!";
        const int txtSize = 30 * scale;
        const int txtW = MeasureText(txt, txtSize);
        const int txtX = static_cast<int>(game.layout.GridStartX()) +
            (Config::Grid::COLS * Config::Grid::CELL_SIZE * scale) / 2 - txtW / 2;
        const int txtY = static_cast<int>(game.layout.GridStartY()) +
            (Config::Grid::ROWS * Config::Grid::CELL_SIZE * scale) / 2 - txtSize / 2;

        DrawRectangle(txtX - 10, txtY - 5, txtW + 20, txtSize + 10, Config::UI::OVERLAY_BG);
        DrawText(txt, txtX, txtY, txtSize, YELLOW);
    }

    if (game.currentState == GameState::Paused) {
        DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Fade(BLACK, 0.6f));
        GameHelpers::DrawCenteredTitle("PAUSED", GetScreenHeight() / 2 - 20, 40, WHITE);
    }
}

} // namespace PlayScreen
