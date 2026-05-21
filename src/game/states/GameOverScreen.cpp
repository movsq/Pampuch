#include "GameOverScreen.h"
#include "game/Game.h"
#include "core/Config.h"
#include "ui/Widgets.h"

namespace GameOverScreen {

void Draw(Game& game) {
    game.activeLevel.Draw(game.layout, /*isEditorMode=*/false);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));

    const bool won = game.activeLevel.CountPoints() == 0;
    const char* text = won ? "STAGE CLEARED!" : "GAME OVER";
    const Color col = won ? GREEN : RED;
    GameHelpers::DrawCenteredTitle(text, GetScreenHeight() / 2 - 80, 40, col);

    DrawText(TextFormat("FINAL SCORE: %d", game.player.score),
        GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 20, 30, YELLOW);

    constexpr float buttonWidth = 200;
    constexpr float buttonHeight = 50;
    constexpr float spacing = 20;
    const float startX = (GetScreenWidth() - (buttonWidth * 2 + spacing)) / 2.0f;
    const float startY = GetScreenHeight() / 2.0f + 40;

    if (UI::Button({ startX, startY, buttonWidth, buttonHeight }, "RETRY", game.layout)) {
        game.activeLevel = game.currentPack.levels[game.currentLevelIndex];
        game.player.lives = Config::Gameplay::STARTING_LIVES;
        game.player.score = 0;
        game.ResetLevelState();
        game.currentState = GameState::Play;
    }
    if (UI::Button({ startX + buttonWidth + spacing, startY, buttonWidth, buttonHeight }, "MENU", game.layout)) {
        game.currentState = GameState::MenuMain;
    }
}

} // namespace GameOverScreen
