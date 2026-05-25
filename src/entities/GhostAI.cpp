// PAMPUCH GHOST AI - implemented from disassembly (PAMPUCH_11 + PAMPUCH_35).
//
// Direction encoding is load-bearing throughout: 1=Right, 2=Down, 3=Left, 4=Up.
// The comparison operators in GetNextBlockedDir (>= for even ghosts, > for odd)
// depend on this numbering. See core/Direction.h.
//
// ASM addresses cited inline are from the IDA disassembly of PAMPUCH_11 and
// PAMPUCH_35. Game-data struct offsets (e.g. [0x187], [0x36D+i*3]) refer to
// the original 16-bit Windows (NE) layout reproduced as C++ fields elsewhere.

#include "Ghost.h"
#include "Player.h"

// PAMPUCH_35 (cseg01:1441) - ghost movement collision check.
//
// Single pass over ghost indices 0..count-1 (ASM byte_7226 loop,
// loc_146D-loc_1551) that produces two outputs:
//
//   - CollisionResult::targetOccupied (ASM var_3, init 0 at loc_1449):
//       another ghost currently occupies the target tile.
//   - CollisionResult::playerBlocked (ASM gameData[0x190], arg_4 target):
//       a player-interaction block fires the effect-queue stub PAMPUCH_40
//       (cseg02:010E) once after the loop.
//
// Caller context verified across four sites - loc_1E3F (Right), loc_1F5D
// (Left), loc_207B (Down), loc_2199 (Up): CheckCollision is invoked per
// candidate direction; arg_A/arg_C are the ghost's current tile and
// arg_E/arg_10 the candidate destination. Caller uses the return value to
// reset byte_722B and reads gameData[0x190] separately.
Ghost::CollisionResult Ghost::CheckCollision_PAMPUCH35(
    int targetX, int targetY, int myId,
    const Player& player, const std::vector<Ghost>& ghosts,
    const EffectCallback& onPlayerInteractionEffect) const
{
    CollisionResult result;  // var_3 / [0x190], both init 0 at loc_1449.

    // loc_146D-loc_1551: single pass over all ghosts.
    for (const Ghost& other : ghosts) {
        if (other.id != myId) {
            // loc_147C: "other ghost" branch.
            // loc_148B-loc_14AC: is the target tile already occupied?
            // (Ghost coords live at [0x36B+i*3], [0x36C+i*3] in the ASM.)
            if (other.gridPos.x == targetX && other.gridPos.y == targetY) {
                result.targetOccupied = true;  // loc_14AE: var_3 = 1.
            }
        } else {
            // loc_14B5: the "self slot" - i == myId.
            // loc_14B5-loc_14CA: does the candidate tile equal the player's tile?
            // Player coords are [0x377], [0x378].
            if (targetX == player.gridPos.x && targetY == player.gridPos.y) {
                // loc_14D9-loc_14E6: compare this ghost's direction
                // [0x36D+myId*3] against the player's direction [0x379].
                if (pampuchDir == player.GetPampuchDirection()) {
                    // loc_14EF-loc_1509: same-direction frame sync check.
                    // ASM compares the player frame counter [0x187] against
                    // this ghost's animFrame [0x189+myId]. Both cycle 0..3,
                    // so the original 'abs(diff) < 1' is equivalent to ==.
                    if (player.frameCounter == animFrame) {
                        result.playerBlocked = true;  // loc_1511.
                    }
                } else {
                    // loc_1517-loc_1525: directions differ.
                    if (player.frameCounter != 0) {
                        result.playerBlocked = true;  // loc_1525.
                    }
                }
            } else {
                // loc_152B-loc_1545: candidate tile is not the player's tile,
                // but the ghost is already standing on the player's tile -
                // block moving away.
                if (gridPos.x == player.gridPos.x && gridPos.y == player.gridPos.y) {
                    result.playerBlocked = true;  // loc_1545.
                }
            }
        }
    }

    // loc_1554: post-loop effect dispatch. ASM pushes ds:offset unk_6980
    // (dseg10:0010) and calls PAMPUCH_40 — the call passes only a pointer, no
    // length, so the consumer self-terminates on the data.
    //
    // Verified against the full chain (producer + consumer):
    //   - PAMPUCH_40 (cseg02:010E) just latches state: dword_7418 = pointer,
    //     byte_7416 = 1 (effect active), word_741C = 0 (cursor). No length.
    //     (Gated on byte_741E != 0, an "effects enabled" flag.)
    //   - PAMPUCH_41 (cseg02:00A9) is the per-tick PC-speaker player. Each tick
    //     it reads one byte b at [dword_7418 + word_741C], advances the cursor,
    //     computes bx = b * 0x21 (a PIT ch.2 divisor), and STOPS when
    //     bx == 0x20DF. 0x20DF / 0x21 == 255, so 0xFF is the unique terminator;
    //     on it the handler clears byte_7416 and silences the speaker (port 61h).
    //
    // So unk_6980 = {5,4,3,2,1} is a descending 5-note jingle (rising pitch as
    // the divisor shrinks), 0xFF-terminated. The 0xFF is consumed as the stop
    // marker, not played. The 0x00 bytes that follow in dseg10 (out to unk_6A80
    // at 0x0110) are segment padding: a 0 byte would compute 0, never hit the
    // 0x20DF terminator, and run the player off the end — so they are provably
    // NOT part of the descriptor. 6 bytes is the whole payload.
    if (result.playerBlocked && onPlayerInteractionEffect) {
        static constexpr std::uint8_t kEffectData_unk6980[] = {
            5, 4, 3, 2, 1, 0xFF  // dseg10:0010 (unk_6980), 0xFF-terminated
        };
        onPlayerInteractionEffect(kEffectData_unk6980, sizeof(kEffectData_unk6980));
    }

    return result;
}

// PAMPUCH_11 (loc_24DE-loc_25A0) - next direction when the current path is
// blocked. Even-indexed ghosts (id 0, 2) use >=; odd-indexed (id 1, 3) use >.
//
// NOTE: in the ASM this writes the new direction directly into
// ghost[i].direction at [0x36D+i*3]; the C++ form returns it and the caller
// assigns into pampuchDir, which is equivalent.
Direction Ghost::GetNextBlockedDir(Direction curDir, int gx, int gy,
    int playerX, int playerY) const noexcept
{
    const int g = id % 4;
    const bool isEven = (g == 0 || g == 2);  // loc_24DE even/odd split.

    if (isEven) {
        // loc_24EC: even-ghost path.
        if (curDir == Direction::Right || curDir == Direction::Left) {
            // loc_2508 (jnb): horizontal motion -> compare y.
            return (gy >= playerY) ? Direction::Up : Direction::Down;
        } else {
            // loc_2559 (jnb): vertical motion -> compare x.
            return (gx >= playerX) ? Direction::Left : Direction::Right;
        }
    } else {
        // loc_25A3 -> loc_25AE: odd-ghost path.
        if (curDir == Direction::Right || curDir == Direction::Left) {
            // loc_25CA (ja): horizontal motion -> compare y.
            return (gy > playerY) ? Direction::Up : Direction::Down;
        } else {
            // loc_261B (ja): vertical motion -> compare x.
            return (gx > playerX) ? Direction::Left : Direction::Right;
        }
    }
}
