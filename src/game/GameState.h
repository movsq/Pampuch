#pragma once

enum class GameState {
    MenuMain,
    MenuPlaySelect,
    MenuEditorMain,
    MenuEditSelect,
    MenuCreateInfo,
    EditorLevelSelect,
    Editor,
    Play,
    Paused,
    GameOver
};

enum class InputFocus {
    None,
    CreatePackName,
    CreateAuthor,
    EditPackName,
    EditAuthor
};
