#pragma once

#include "raylib.h"
#include "core/GridCoord.h"
#include "core/Direction.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

class Level;
class Player;
class Layout;

class Ghost {
public:
    // Two-output result for the PAMPUCH_35 collision check (cseg01:1441).
    // The ASM returns one value (var_3) and writes a second through arg_4
    // (the byte at gameData[0x190]); we model both explicitly.
    struct CollisionResult {
        bool targetOccupied = false;  // ASM var_3 / loc_14AE
        bool playerBlocked  = false;  // ASM [0x190] / arg_4 target
    };

    // PAMPUCH_40 effect-queue stub (cseg02:010E). When invoked, the original
    // stores a far pointer (ds:offset) to effect descriptor data into
    // dword_7418, sets byte_7416 = 1, and zeroes word_741C; an external tick
    // handler reads through that pointer and renders the effect.
    // The C++ callback receives the equivalent: a pointer to the effect data
    // and its length. At the PAMPUCH_35+0x121 call site, the data is always
    // unk_6980 = {5, 4, 3, 2, 1, 0xFF, 0, 0, 0, 0} (dseg10:0010).
    using EffectCallback = std::function<void(const std::uint8_t* effectData, std::size_t dataLen)>;
    GridCoord gridPos{ 0, 0 };
    Vector2 pixelPos{ 0.0f, 0.0f };
    Vector2 targetPos{ 0.0f, 0.0f };
    bool isMoving = false;
    Vector2 currentDir{ 0.0f, 0.0f };
    int id = 0;
    Color color = RED;
    Direction pampuchDir = Direction::Right;
    int animFrame = 0;

    static constexpr std::array<Color, 6> COLORS = {{
        RED, PINK, SKYBLUE, ORANGE, LIME, VIOLET
    }};

    Ghost() = default;
    Ghost(int startX, int startY, int ghostId, Color c, const Layout& layout);

    // PAMPUCH_35 collision check (cseg01:1441). Returns both outputs from the
    // original (return value + arg_4 flag) and invokes the supplied callback
    // for the post-loop PAMPUCH_40 effect-queue hook. See GhostAI.cpp.
    [[nodiscard]] CollisionResult CheckCollision_PAMPUCH35(
        int targetX, int targetY, int myId,
        const Player& player, const std::vector<Ghost>& ghosts,
        const EffectCallback& onPlayerInteractionEffect) const;

    // loc_24DE next-direction-when-blocked (preserved original behavior).
    [[nodiscard]] Direction GetNextBlockedDir(Direction curDir, int gx, int gy,
        int playerX, int playerY) const noexcept;

    void Update(float dt, const Level& level, const Player& player,
        const std::vector<Ghost>& allGhosts, const Layout& layout, bool turboMode);

    void Draw(Vector2 lookTarget, const Layout& layout) const;

private:
    float m_blinkTimer = 0.0f;
    float m_timeUntilNextBlink = 3.0f;
    bool m_isBlinking = false;

    void DrawFace(Vector2 center, float centerX, float centerY, float bodyOffset,
        float tilt, Vector2 lookTarget, const Layout& layout) const;
};
