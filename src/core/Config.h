#pragma once

#include "raylib.h"
#include <cstdint>

namespace Config {

    namespace Grid {
        constexpr int CELL_SIZE = 32;
        constexpr int COLS = 24;
        constexpr int ROWS = 16;
        constexpr int BASE_SIDEBAR_WIDTH = 250;
        constexpr int MARGIN = 20;
    }

    namespace Physics {
        constexpr float SPEED_MULTIPLIER_NORMAL = 1.0f;
        constexpr float SPEED_MULTIPLIER_TURBO = 1.2f;
        constexpr float BASE_MOVE_SPEED = 170.0f;
        constexpr float BASE_GHOST_SPEED = 170.0f;
        constexpr float COLLISION_RADIUS = 12.0f;
    }

    namespace Gameplay {
        constexpr int MAX_GHOSTS = 4;
        constexpr int SCORE_PER_DOT = 10;
        constexpr int POINTS_FOR_EXTRA_LIFE = 1000;
        constexpr int STARTING_LIVES = 5;
        constexpr int GHOST_ANIM_FRAMES = 4;
        constexpr int LIFE_WARNING_THRESHOLD = 2;
    }

    namespace Render {
        constexpr float GHOST_BODY_OFFSET = -2.0f;
        constexpr float GHOST_BODY_MARGIN = 4.0f;
        constexpr float GHOST_TILT_SPEED = 8.0f;
        constexpr float GHOST_TILT_AMPLITUDE = 7.0f;
        constexpr float GHOST_PHASE_OFFSET = 2.5f;
        constexpr int GHOST_LEG_COUNT = 3;

        constexpr float GHOST_EYE_OFFSET_X = 4.0f;
        constexpr float GHOST_EYE_OFFSET_Y = -2.0f;
        constexpr float GHOST_EYE_SIZE = 4.0f;
        constexpr float GHOST_PUPIL_SIZE = 2.0f;
        constexpr float GHOST_PUPIL_MOVE_RADIUS = 1.8f;
        constexpr float GHOST_NOSE_OFFSET = 4.0f;
        constexpr float GHOST_NOSE_SIZE = 2.0f;
        constexpr float GHOST_BLINK_LINE_WIDTH = 3.0f;
        constexpr float GHOST_BLINK_LINE_THICKNESS = 2.0f;

        constexpr float PLAYER_BODY_MARGIN = 1.0f;
        constexpr float PLAYER_CENTER_OFFSET = 1.0f;
        constexpr float PLAYER_MOUTH_OPEN_MAX = 45.0f;
        constexpr float PLAYER_MOUTH_OPEN_IDLE = 20.0f;
        constexpr int PLAYER_MOUTH_SEGMENTS = 16;
        constexpr float PLAYER_EYE_SIZE = 2.5f;
        constexpr float PLAYER_EYE_OFFSET_MAJOR = 7.0f;
        constexpr float PLAYER_EYE_OFFSET_MINOR = 3.0f;
        constexpr float PLAYER_BLINK_LINE_WIDTH = 3.0f;
        constexpr float PLAYER_BLINK_LINE_THICKNESS = 1.5f;

        constexpr float POINT_RADIUS = 4.0f;
        constexpr float WALL_OUTLINE_THICKNESS = 1.0f;
        constexpr float GRID_LINE_THICKNESS = 1.0f;
        constexpr uint8_t GRID_LINE_ALPHA = 30;

        constexpr float EDITOR_START_MARKER_THICKNESS = 2.0f;
        constexpr int EDITOR_START_FONT_SIZE = 10;
        constexpr float TOOL_ICON_SIZE = 30.0f;
        constexpr float TOOL_ICON_PADDING = 5.0f;
        constexpr float TOOL_ICON_OFFSET_Y = 7.0f;
        constexpr float TOOL_POINT_RADIUS = 6.0f;
    }

    namespace Anim {
        constexpr float BLINK_DURATION = 0.15f;
        constexpr float MIN_BLINK_INTERVAL_GHOST = 1.0f;
        constexpr float MAX_BLINK_INTERVAL_GHOST = 6.0f;
        constexpr float MIN_BLINK_INTERVAL_PLAYER = 2.0f;
        constexpr float MAX_BLINK_INTERVAL_PLAYER = 5.0f;
        constexpr float PLAYER_ANIM_SPEED = 15.0f;
    }

    namespace UI {
        constexpr int MAX_FONT_SIZE = 40;
        constexpr int MAX_SIDEBAR_FONT_SIZE = 30;
        constexpr int BASE_FONT_SIZE = 20;
        constexpr int SMALL_FONT_SIZE = 10;
        constexpr float BUTTON_HEIGHT = 45.0f;
        constexpr float BUTTON_GAP = 5.0f;
        constexpr float BUTTON_BORDER_NORMAL = 2.0f;
        constexpr float BUTTON_ACCENT_WIDTH = 4.0f;
        constexpr float BUTTON_CORNER_ROUNDNESS = 0.18f;
        constexpr int HUD_BAR_BASE_HEIGHT = 72;
        constexpr float MENU_BUTTON_SCALE_THRESHOLD = 600;
        constexpr float MENU_BUTTON_SCALE_FACTOR = 1.5f;
        constexpr float MENU_START_Y_SCALE_FACTOR = 1.2f;
        constexpr size_t MAX_INPUT_LENGTH = 60;
        constexpr size_t MAX_NAME_LENGTH = 30;
        constexpr int PRINTABLE_CHAR_MIN = 32;
        constexpr int PRINTABLE_CHAR_MAX = 125;

        constexpr Color BUTTON_HOVER_BG = { 60, 60, 60, 255 };
        constexpr Color BUTTON_NORMAL_BG = { 40, 40, 40, 255 };
        constexpr Color SIDEBAR_BG = { 30, 30, 30, 255 };
        constexpr Color DARK_BG = { 20, 20, 20, 255 };
        constexpr Color DARKER_BG = { 10, 10, 10, 255 };
        constexpr Color OVERLAY_BG = { 0, 0, 0, 180 };
    }

    namespace Window {
        constexpr int INITIAL_WIDTH = 1280;
        constexpr int INITIAL_HEIGHT = 768;
        constexpr int MIN_REFRESH_RATE = 60;
        constexpr const char* TITLE = "Pampuch";
    }
}
