#pragma once

class Game;

namespace MenuScreens {
    void DrawMenuMain(Game& game);
    void DrawMenuEditorMain(Game& game);
    void DrawPackSelect(Game& game, bool isEdit);
    void DrawMenuCreateInfo(Game& game);
}
