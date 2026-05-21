#include "Player.h"
#include "core/Utils.h"
#include "render/Layout.h"
#include "world/Level.h"
#include <cmath>

void Player::Reset(GridCoord startPos, const Layout& layout) {
    gridPos = startPos;
    pixelPos = layout.GetScreenPos(gridPos);
    targetPos = pixelPos;
    isMoving = false;
    currentDir = { 0.0f, 0.0f };
    queuedDir = { 0.0f, 0.0f };
    facingAngle = 0.0f;
    frameCounter = 0;
    m_bufferedInput = { 0.0f, 0.0f };
    m_timeUntilNextBlink = Utils::RandomFloat(
        Config::Anim::MIN_BLINK_INTERVAL_PLAYER,
        Config::Anim::MAX_BLINK_INTERVAL_PLAYER);
}

Direction Player::GetPampuchDirection() const noexcept {
    if (currentDir.x > 0) return Direction::Right;
    if (currentDir.y > 0) return Direction::Down;
    if (currentDir.x < 0) return Direction::Left;
    if (currentDir.y < 0) return Direction::Up;
    return Direction::None;
}

void Player::Update(float dt, Level& level, const Layout& layout, bool& waitingForStart, bool turboMode) {
    UpdateBlinkAnimation(dt);

    if (isMoving) {
        m_animTimer += dt * Config::Anim::PLAYER_ANIM_SPEED;
    } else {
        m_animTimer = 0.0f;
    }

    const Vector2 input = GetInputDirection();

    if (waitingForStart) {
        if (input.x != 0 || input.y != 0) {
            waitingForStart = false;
            queuedDir = input;
            currentDir = input;
        } else {
            return;
        }
    }

    if (input.x != 0 || input.y != 0) queuedDir = input;

    if (isMoving && (input.x != 0 || input.y != 0)) {
        if (input.x == -currentDir.x && input.y == -currentDir.y) {
            gridPos.x -= static_cast<int>(currentDir.x);
            gridPos.y -= static_cast<int>(currentDir.y);
            targetPos = layout.GetScreenPos(gridPos);
            currentDir = input;
            UpdateFacingAngle(input);
        }
    }

    const float baseSpeed = Config::Physics::BASE_MOVE_SPEED;
    const float multiplier = turboMode ? Config::Physics::SPEED_MULTIPLIER_TURBO
                                       : Config::Physics::SPEED_MULTIPLIER_NORMAL;
    const float scaledSpeed = baseSpeed * multiplier * layout.Scale();
    const float moveBudget = scaledSpeed * dt;

    if (!isMoving) {
        TryStartMovement(level, layout);
    }

    if (isMoving) {
        ProcessMovement(moveBudget, level, layout);
    }
}

void Player::Draw(const Layout& layout) const {
    const float scaledCell = layout.GetScaledCellSize();
    const float s = layout.GetScaledValue(1.0f);

    const Vector2 center = {
        (pixelPos.x + scaledCell / 2.0f) - Config::Render::PLAYER_CENTER_OFFSET + 1.0f,
        pixelPos.y + scaledCell / 2.0f
    };
    const float radius = (scaledCell / 2.0f) - (Config::Render::PLAYER_BODY_MARGIN * s);

    const float mouthOpenAngle = isMoving
        ? std::abs(std::sin(m_animTimer)) * Config::Render::PLAYER_MOUTH_OPEN_MAX
        : Config::Render::PLAYER_MOUTH_OPEN_IDLE;

    DrawCircleSector(center, radius,
        facingAngle + mouthOpenAngle,
        facingAngle + 360.0f - mouthOpenAngle,
        Config::Render::PLAYER_MOUTH_SEGMENTS, YELLOW);

    Vector2 eyePos = center;
    const int angleInt = static_cast<int>(facingAngle);
    const float majorOff = Config::Render::PLAYER_EYE_OFFSET_MAJOR * s;
    const float minorOff = Config::Render::PLAYER_EYE_OFFSET_MINOR * s;

    if (angleInt == 0)   { eyePos.x += minorOff; eyePos.y -= majorOff; }
    if (angleInt == 180) { eyePos.x -= minorOff; eyePos.y -= majorOff; }
    if (angleInt == 90)  { eyePos.x += majorOff; eyePos.y -= minorOff; }
    if (angleInt == 270) { eyePos.x += majorOff; eyePos.y -= minorOff; }

    if (!m_isBlinking) {
        DrawCircleV(eyePos, Config::Render::PLAYER_EYE_SIZE * s, BLACK);
    } else {
        const float blinkW = Config::Render::PLAYER_BLINK_LINE_WIDTH * s;
        DrawLineEx(
            { eyePos.x - blinkW, eyePos.y },
            { eyePos.x + blinkW, eyePos.y },
            Config::Render::PLAYER_BLINK_LINE_THICKNESS * s, BLACK);
    }
}

void Player::UpdateBlinkAnimation(float dt) {
    m_blinkTimer += dt;
    if (!m_isBlinking && m_blinkTimer >= m_timeUntilNextBlink) {
        m_isBlinking = true;
        m_blinkTimer = 0.0f;
    } else if (m_isBlinking && m_blinkTimer >= Config::Anim::BLINK_DURATION) {
        m_isBlinking = false;
        m_blinkTimer = 0.0f;
        m_timeUntilNextBlink = Utils::RandomFloat(
            Config::Anim::MIN_BLINK_INTERVAL_PLAYER,
            Config::Anim::MAX_BLINK_INTERVAL_PLAYER);
    }
}

Vector2 Player::GetInputDirection() noexcept {
    // Latch the most recently requested direction and keep it until the player
    // asks for a *different* one. Re-pressing the current heading must not
    // overwrite a pending perpendicular turn (e.g. holding Up while moving
    // right shouldn't be cancelled by a stray Right tap).
    Vector2 pressed = { 0.0f, 0.0f };
    if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) pressed = { 0.0f, -1.0f };
    if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) pressed = { 0.0f,  1.0f };
    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) pressed = { -1.0f, 0.0f };
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) pressed = {  1.0f, 0.0f };

    const bool gotPress = (pressed.x != 0.0f || pressed.y != 0.0f);
    const bool matchesCurrent = (pressed.x == currentDir.x && pressed.y == currentDir.y);
    if (gotPress && !matchesCurrent) {
        m_bufferedInput = pressed;
    }

    return m_bufferedInput;
}

void Player::UpdateFacingAngle(Vector2 dir) noexcept {
    if (dir.x > 0) facingAngle = 0.0f;
    else if (dir.x < 0) facingAngle = 180.0f;
    else if (dir.y > 0) facingAngle = 90.0f;
    else if (dir.y < 0) facingAngle = 270.0f;
}

void Player::TryStartMovement(const Level& level, const Layout& layout) {
    Vector2 bestDir = { 0.0f, 0.0f };
    const int qx = gridPos.x + static_cast<int>(queuedDir.x);
    const int qy = gridPos.y + static_cast<int>(queuedDir.y);
    const int cx = gridPos.x + static_cast<int>(currentDir.x);
    const int cy = gridPos.y + static_cast<int>(currentDir.y);

    if ((queuedDir.x != 0 || queuedDir.y != 0) && level.IsWalkable(qx, qy)) {
        bestDir = queuedDir;
        currentDir = bestDir;
    } else if ((currentDir.x != 0 || currentDir.y != 0) && level.IsWalkable(cx, cy)) {
        bestDir = currentDir;
    }

    if (bestDir.x != 0 || bestDir.y != 0) {
        gridPos.x += static_cast<int>(bestDir.x);
        gridPos.y += static_cast<int>(bestDir.y);
        targetPos = layout.GetScreenPos(gridPos);
        isMoving = true;
        UpdateFacingAngle(bestDir);
    }
}

void Player::ProcessMovement(float moveBudget, Level& level, const Layout& layout) {
    const bool movingX = currentDir.x != 0;
    const bool movingY = currentDir.y != 0;

    if (movingX) pixelPos.y = targetPos.y;
    if (movingY) pixelPos.x = targetPos.x;

    const float distX = targetPos.x - pixelPos.x;
    const float distY = targetPos.y - pixelPos.y;
    const float distToTarget = movingX ? std::abs(distX) : std::abs(distY);

    const float halfCell = layout.GetScaledCellSize() / 2.0f;
    if (distToTarget <= halfCell) {
        if (level.GetTile(gridPos) == TileType::Point) {
            level.SetTile(gridPos, TileType::Floor);
            score += Config::Gameplay::SCORE_PER_DOT;
            if (score >= scoreThresholdForLife) {
                ++lives;
                scoreThresholdForLife += Config::Gameplay::POINTS_FOR_EXTRA_LIFE;
            }
        }
    }

    if (distToTarget > moveBudget) {
        if (movingX) pixelPos.x += (distX > 0 ? 1.0f : -1.0f) * moveBudget;
        if (movingY) pixelPos.y += (distY > 0 ? 1.0f : -1.0f) * moveBudget;
    } else {
        pixelPos = targetPos;
        // PAMPUCH player frame counter tick (ASM [0x187], PAMPUCH_11
        // loc_2677-loc_268D). Cycles 0->1->2->3->0 on tile-step completion.
        // Mirrors the ghost animFrame tick in Ghost.cpp.
        frameCounter = (frameCounter + 1) % 4;
        const float remainingMove = moveBudget - distToTarget;
        Vector2 nextDir = { 0.0f, 0.0f };
        bool canContinue = false;

        const int qx = gridPos.x + static_cast<int>(queuedDir.x);
        const int qy = gridPos.y + static_cast<int>(queuedDir.y);
        if ((queuedDir.x != 0 || queuedDir.y != 0) && level.IsWalkable(qx, qy)) {
            nextDir = queuedDir;
            canContinue = true;
        } else {
            const int cx = gridPos.x + static_cast<int>(currentDir.x);
            const int cy = gridPos.y + static_cast<int>(currentDir.y);
            if (level.IsWalkable(cx, cy)) {
                nextDir = currentDir;
                canContinue = true;
            }
        }

        if (canContinue) {
            currentDir = nextDir;
            gridPos.x += static_cast<int>(nextDir.x);
            gridPos.y += static_cast<int>(nextDir.y);
            targetPos = layout.GetScreenPos(gridPos);
            pixelPos.x += nextDir.x * remainingMove;
            pixelPos.y += nextDir.y * remainingMove;
            UpdateFacingAngle(nextDir);
        } else {
            isMoving = false;
        }
    }
}
