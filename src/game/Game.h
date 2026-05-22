#pragma once

#include "GameState.h"
#include "audio/AudioManager.h"
#include "editor/Editor.h"
#include "entities/Ghost.h"
#include "entities/Player.h"
#include "render/Layout.h"
#include "render/PostProcess.h"
#include "world/Level.h"
#include "world/MapPack.h"

#include <string>
#include <vector>

class Game {
public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void Run();

    // Shared state — screens read/write these.
    GameState currentState = GameState::MenuMain;
    MapPack currentPack;
    int currentLevelIndex = 0;
    Level activeLevel;

    Player player;
    std::vector<Ghost> activeGhosts;

    Editor editor;
    Layout layout;
    PostProcess postProcess;
    AudioManager audio;

    bool waitingForStart = true;
    bool turboMode = false;
    bool infiniteLives = false;

    std::string inputNameBuffer;
    std::string inputAuthorBuffer;
    InputFocus inputFocus = InputFocus::None;

    void ResetLevelState();

private:
    void LoadWindowIcon();
    void HandleGlobalInput();
    void Update(float dt);
    void Draw();
    void DrawCopyright() const;
};

namespace GameHelpers {
    void DrawCenteredTitle(const char* text, int yOffset, int fontSize, Color color);
}
