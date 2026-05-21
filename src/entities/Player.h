#pragma once

#include "raylib.h"
#include "core/GridCoord.h"
#include "core/Direction.h"
#include "core/Config.h"

class Level;
class Layout;

class Player {
public:
    GridCoord gridPos{ 0, 0 };
    Vector2 pixelPos{ 0.0f, 0.0f };
    Vector2 targetPos{ 0.0f, 0.0f };
    bool isMoving = false;
    Vector2 currentDir{ 0.0f, 0.0f };
    Vector2 queuedDir{ 0.0f, 0.0f };
    float facingAngle = 0.0f;
    int lives = Config::Gameplay::STARTING_LIVES;
    int score = 0;
    int scoreThresholdForLife = Config::Gameplay::POINTS_FOR_EXTRA_LIFE;

    // PAMPUCH player frame counter (ASM offset [0x187]). Cycles 0->1->2->3->0
    // on each tile-step completion. Read by Ghost::CheckCollision_PAMPUCH35.
    int frameCounter = 0;

    void Reset(GridCoord startPos, const Layout& layout);
    void Update(float dt, Level& level, const Layout& layout, bool& waitingForStart, bool turboMode);
    void Draw(const Layout& layout) const;

    // Returns the original Pampuch direction code for AI interaction.
    [[nodiscard]] Direction GetPampuchDirection() const noexcept;

private:
    float m_animTimer = 0.0f;
    float m_blinkTimer = 0.0f;
    float m_timeUntilNextBlink = 3.0f;
    bool m_isBlinking = false;

    Vector2 m_bufferedInput{ 0.0f, 0.0f };

    void UpdateBlinkAnimation(float dt);
    [[nodiscard]] Vector2 GetInputDirection() noexcept;
    void UpdateFacingAngle(Vector2 dir) noexcept;
    void TryStartMovement(const Level& level, const Layout& layout);
    void ProcessMovement(float moveBudget, Level& level, const Layout& layout);
};
