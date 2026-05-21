#pragma once

#include "ToolDefinition.h"
#include <string>
#include <vector>

class Level;
class Layout;

class Editor {
public:
    EditorTool currentTool = EditorTool::Wall;

    static const std::vector<ToolDefinition>& GetTools();

    void HandleInput(Level& level, const Layout& layout);
    void DrawSidebar(const std::string& packName, int levelIdx, int ghostCount, const Layout& layout);

private:
    void HandleToolSelection();
    void HandleErase(Level& level, int mx, int my);
    void RemoveGhostSpawnAt(Level& level, int x, int y);
    void ApplyTool(int mx, int my, Level& level);
    void DrawToolIcon(const ToolDefinition& tool, Rectangle iconRect, const Layout& layout) const;
};
