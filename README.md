# Pampuch Remake

A modernized clone of **Pampuch** (released by ZONER software in 1994, author
Milan Behro) — a Czech Pac-Man-style arcade game — with ghost AI
reverse-engineered from the original DOS executable and a built-in level
editor.

Pampuch itself was a remake of **Pampúch a Duch** by Dušan Blaško (1986).

## Download

See the [Releases](../../releases) page for a pre-built binary.

## Building from source

Requirements: CMake 3.20+, a C++20 compiler, OpenGL, and git (CMake fetches
raylib on first configure). Works on Linux, macOS, and Windows (MSVC or MinGW)
with no extra packages.

```
cmake -S . -B build
cmake --build build --config Release
```

The first configure clones raylib 5.5 into `build/_deps/` and builds it as
part of the project — adds ~1–2 minutes once, then it's cached. The
executable lands in `build/` (or `build/Release/` on multi-config generators).
Runtime assets (`shaders/`, `resources/`, `maps/`) are copied next to it
automatically.

On Linux you may also need the usual raylib dev headers for X11/Wayland/ALSA;
on Ubuntu: `sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev`.

## Reverse engineering

Ghost behavior is not a re-imagining — it is a port of the original. The AI in
`src/entities/GhostAI.cpp` was reconstructed from an IDA disassembly of the
original binaries (`PAMPUCH_11` and `PAMPUCH_35`), with ASM addresses cited
inline next to the C++ that replaces them. A few quirks worth knowing:

- **Direction encoding is load-bearing.** `1=Right, 2=Down, 3=Left, 4=Up`
  (`core/Direction.h`). The decision routines compare directions with `>=` for
  even-indexed ghosts and `>` for odd-indexed ones, so renumbering would
  silently change which ghost turns which way.
- **Original game-data offsets are preserved** as named C++ fields, so cross-
  referencing the disassembly stays straightforward (e.g. the old
  `gameData[0x190]` player-interaction flag, the `[0x36D + i*3]` per-ghost
  records).
- **Per-direction collision passes** (`loc_1E3F`/`loc_1F5D`/`loc_207B`/
  `loc_2199` in the original) are kept as four call sites with the same
  semantics rather than collapsed into a loop, to match observable behavior on
  contested tiles.

The default level pack shipped in `maps/Zoner Map Pack.json` recreates the
layouts from Zoner's Pampuch. New maps can be authored in-game via the level editor
(`src/editor/`).

## Sound

The source code contains sound playback functionality but it is not currently
wired up — no audio plays at runtime. The implementation is in place should
sound effects be added back later.
