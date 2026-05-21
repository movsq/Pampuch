#pragma once

#include "raylib.h"
#include <string>

class Layout;

namespace UI {

    [[nodiscard]] bool Button(Rectangle bounds, const std::string& text, const Layout& layout);
    void HandleTextInput(std::string& buffer, size_t maxLength);

}
