#include "Ghost.h"
#include "Player.h"
#include "core/Config.h"
#include "core/Utils.h"
#include "render/Layout.h"
#include "world/Level.h"
#include <cmath>

Ghost::Ghost(int startX, int startY, int ghostId, Color c, const Layout& layout)
    : gridPos{ startX, startY }
    , id(ghostId)
    , color(c)
{
    pixelPos  = layout.GetScreenPos(gridPos);
    targetPos = pixelPos;

    // Initial directions per ghost id (preserved from original behavior).
    switch (id) {
        case 0: currentDir = { 1, 0 };  pampuchDir = Direction::Right; break;
        case 1: currentDir = { -1, 0 }; pampuchDir = Direction::Left;  break;
        case 2: currentDir = { 0, -1 }; pampuchDir = Direction::Up;    break;
        case 3: currentDir = { 0, -1 }; pampuchDir = Direction::Up;    break;
        default: currentDir = { 1, 0 }; pampuchDir = Direction::Right; break;
    }

    m_timeUntilNextBlink = Utils::RandomFloat(
        Config::Anim::MIN_BLINK_INTERVAL_GHOST,
        Config::Anim::MAX_BLINK_INTERVAL_GHOST);
}

void Ghost::Update(float dt, const Level& level, const Player& player,
    const std::vector<Ghost>& allGhosts, const Layout& layout, bool turboMode)
{
    // Blink animation
    m_blinkTimer += dt;
    if (!m_isBlinking && m_blinkTimer >= m_timeUntilNextBlink) {
        m_isBlinking = true;
        m_blinkTimer = 0.0f;
    } else if (m_isBlinking && m_blinkTimer >= Config::Anim::BLINK_DURATION) {
        m_isBlinking = false;
        m_blinkTimer = 0.0f;
        m_timeUntilNextBlink = Utils::RandomFloat(
            Config::Anim::MIN_BLINK_INTERVAL_GHOST,
            Config::Anim::MAX_BLINK_INTERVAL_GHOST);
    }

    const float baseSpeed = Config::Physics::BASE_GHOST_SPEED;
    const float multiplier = turboMode ? Config::Physics::SPEED_MULTIPLIER_TURBO
                                       : Config::Physics::SPEED_MULTIPLIER_NORMAL;
    const float scaledSpeed = baseSpeed * multiplier * layout.Scale();
    const float moveBudget = scaledSpeed * dt;

    if (isMoving) {
        const float distX = targetPos.x - pixelPos.x;
        const float distY = targetPos.y - pixelPos.y;
        const float distToTarget = std::abs(distX) + std::abs(distY);

        if (distToTarget > moveBudget) {
            if (std::abs(distX) > 0) pixelPos.x += (distX > 0 ? 1.0f : -1.0f) * moveBudget;
            if (std::abs(distY) > 0) pixelPos.y += (distY > 0 ? 1.0f : -1.0f) * moveBudget;
        } else {
            pixelPos = targetPos;
            isMoving = false;
            animFrame = (animFrame + 1) % Config::Gameplay::GHOST_ANIM_FRAMES;
        }
    }

    if (!isMoving) {
        const GridCoord dirOffset = DirectionToOffset(pampuchDir);
        const int nextX = gridPos.x + dirOffset.x;
        const int nextY = gridPos.y + dirOffset.y;

        bool pathBlocked = false;

        if (!level.IsWithinBounds(nextX, nextY) || !level.IsWalkable(nextX, nextY)) {
            pathBlocked = true;
        }

        if (!pathBlocked) {
            const CollisionResult col = CheckCollision_PAMPUCH35(
                nextX, nextY, id, player, allGhosts,
                // TODO: wire PAMPUCH_40 effect queue (unk_6980); see GhostAI.cpp.
                [](const std::uint8_t*, std::size_t) {});
            if (col.targetOccupied) {
                pathBlocked = true;
            }
        }

        if (pathBlocked) {
            pampuchDir = GetNextBlockedDir(pampuchDir, gridPos.x, gridPos.y,
                player.gridPos.x, player.gridPos.y);
        } else {
            gridPos.x = nextX;
            gridPos.y = nextY;
            targetPos = layout.GetScreenPos(gridPos);
            isMoving = true;
        }
    }
}

void Ghost::Draw(Vector2 lookTarget, const Layout& layout) const {
    const float scaledCell = layout.GetScaledCellSize();
    const float s = layout.GetScaledValue(1.0f);

    const float centerX = pixelPos.x + scaledCell / 2.0f;
    const float centerY = pixelPos.y + scaledCell / 2.0f;
    const Vector2 center = { centerX, centerY };

    const float radius = (scaledCell / 2.0f) - (Config::Render::GHOST_BODY_MARGIN * s);
    const float bodyOffset = Config::Render::GHOST_BODY_OFFSET * s;

    float tilt = 0.0f;
    if (isMoving) {
        const float phaseOffset = id * Config::Render::GHOST_PHASE_OFFSET;
        tilt = std::sin((static_cast<float>(GetTime()) * Config::Render::GHOST_TILT_SPEED) + phaseOffset)
             * (Config::Render::GHOST_TILT_AMPLITUDE * s);
    }

    const float topY = centerY + bodyOffset;
    const float botY = centerY + bodyOffset + radius;

    Vector2 points[4] = {
        { centerX - radius, topY },
        { centerX + radius, topY },
        { centerX - radius, botY },
        { centerX + radius, botY }
    };

    for (auto& p : points) p = Utils::RotatePoint(p, center, tilt);

    DrawTriangle(points[0], points[2], points[1], color);
    DrawTriangle(points[1], points[2], points[3], color);

    const Vector2 headPos = Utils::RotatePoint({ centerX, centerY + bodyOffset }, center, tilt);
    DrawCircleV(headPos, radius, color);

    const float legRadius = radius / Config::Render::GHOST_LEG_COUNT;
    for (int i = 0; i < Config::Render::GHOST_LEG_COUNT; ++i) {
        const float legBaseX = (centerX - radius) + (legRadius * 2 * i) + legRadius;
        const Vector2 legPos = Utils::RotatePoint({ legBaseX, botY }, center, tilt);
        DrawCircleV(legPos, legRadius, color);
    }

    DrawFace(center, centerX, centerY, bodyOffset, tilt, lookTarget, layout);
}

void Ghost::DrawFace(Vector2 center, float centerX, float centerY, float bodyOffset,
    float tilt, Vector2 lookTarget, const Layout& layout) const
{
    const float s = layout.GetScaledValue(1.0f);
    const float eyeOffX = Config::Render::GHOST_EYE_OFFSET_X * s;
    const float eyeOffY = bodyOffset + (Config::Render::GHOST_EYE_OFFSET_Y * s);

    const Vector2 leftEyePos  = Utils::RotatePoint({ centerX - eyeOffX, centerY + eyeOffY }, center, tilt);
    const Vector2 rightEyePos = Utils::RotatePoint({ centerX + eyeOffX, centerY + eyeOffY }, center, tilt);
    const Vector2 nosePos     = Utils::RotatePoint(
        { centerX, centerY + bodyOffset + (Config::Render::GHOST_NOSE_OFFSET * s) }, center, tilt);

    DrawCircleV(nosePos, Config::Render::GHOST_NOSE_SIZE * s, BLACK);

    if (m_isBlinking) {
        const float w = Config::Render::GHOST_BLINK_LINE_WIDTH * s;
        const Vector2 lStart = Utils::RotatePoint({ centerX - eyeOffX - w, centerY + eyeOffY }, center, tilt);
        const Vector2 lEnd   = Utils::RotatePoint({ centerX - eyeOffX + w, centerY + eyeOffY }, center, tilt);
        const Vector2 rStart = Utils::RotatePoint({ centerX + eyeOffX - w, centerY + eyeOffY }, center, tilt);
        const Vector2 rEnd   = Utils::RotatePoint({ centerX + eyeOffX + w, centerY + eyeOffY }, center, tilt);

        DrawLineEx(lStart, lEnd, Config::Render::GHOST_BLINK_LINE_THICKNESS * s, BLACK);
        DrawLineEx(rStart, rEnd, Config::Render::GHOST_BLINK_LINE_THICKNESS * s, BLACK);
    } else {
        const float eyeSize = Config::Render::GHOST_EYE_SIZE * s;
        DrawCircleV(leftEyePos,  eyeSize, WHITE);
        DrawCircleV(rightEyePos, eyeSize, WHITE);

        const float dx = lookTarget.x - centerX;
        const float dy = lookTarget.y - centerY;
        const float lookAngle = std::atan2(dy, dx);

        const float pupilMove = Config::Render::GHOST_PUPIL_MOVE_RADIUS * s;
        const float pOffX = std::cos(lookAngle) * pupilMove;
        const float pOffY = std::sin(lookAngle) * pupilMove;

        const float pupilSize = Config::Render::GHOST_PUPIL_SIZE * s;
        DrawCircleV({ leftEyePos.x  + pOffX, leftEyePos.y  + pOffY }, pupilSize, BLACK);
        DrawCircleV({ rightEyePos.x + pOffX, rightEyePos.y + pOffY }, pupilSize, BLACK);
    }
}
