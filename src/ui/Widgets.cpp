#include "Widgets.h"
#include "core/Config.h"
#include "render/Layout.h"

namespace UI {

bool Button(Rectangle bounds, const std::string& text, const Layout& layout) {
    const Vector2 mousePoint = GetMousePosition();
    const bool isHovered = CheckCollisionPointRec(mousePoint, bounds);

    const Color fillColor = isHovered ? Config::UI::BUTTON_HOVER_BG : Config::UI::BUTTON_NORMAL_BG;
    const Color borderColor = isHovered ? Fade(LIGHTGRAY, 0.85f) : Fade(LIGHTGRAY, 0.35f);
    const Color accentColor = borderColor;

    const float roundness = Config::UI::BUTTON_CORNER_ROUNDNESS;
    const int segments = 6;

    DrawRectangleRounded(bounds, roundness, segments, fillColor);
    DrawRectangleRoundedLinesEx(bounds, roundness, segments, Config::UI::BUTTON_BORDER_NORMAL, borderColor);

    const float accentInset = 4.0f;
    const Rectangle accent = {
        bounds.x + accentInset,
        bounds.y + accentInset,
        Config::UI::BUTTON_ACCENT_WIDTH,
        bounds.height - accentInset * 2.0f
    };
    if (accent.height > 0.0f) {
        DrawRectangleRounded(accent, 1.0f, 4, accentColor);
    }

    int fontSize = Config::UI::BASE_FONT_SIZE * layout.Scale();
    if (fontSize > Config::UI::MAX_FONT_SIZE) fontSize = Config::UI::MAX_FONT_SIZE;

    // Center text in the area to the right of the accent stripe so labels never bleed into it.
    const float textRegionLeft = accent.x + accent.width + 6.0f;
    const float textRegionRight = bounds.x + bounds.width - 6.0f;
    const int textWidth = MeasureText(text.c_str(), fontSize);
    DrawText(text.c_str(),
        static_cast<int>(textRegionLeft + (textRegionRight - textRegionLeft - textWidth) / 2.0f),
        static_cast<int>(bounds.y + (bounds.height / 2 - fontSize / 2)),
        fontSize, WHITE);

    return isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void HandleTextInput(std::string& buffer, size_t maxLength) {
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= Config::UI::PRINTABLE_CHAR_MIN) &&
            (key <= Config::UI::PRINTABLE_CHAR_MAX) &&
            (buffer.length() < maxLength)) {
            buffer += static_cast<char>(key);
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !buffer.empty()) {
        buffer.pop_back();
    }
}

} // namespace UI
