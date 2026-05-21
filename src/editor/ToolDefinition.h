#pragma once

#include "raylib.h"
#include <string>

enum class EditorTool { Wall, Floor, Point, Fill, ClearPoints, Start, Ghost };

struct ToolDefinition {
    EditorTool type;
    std::string name;
    std::string key;
    std::string description;
    Color color;
};
