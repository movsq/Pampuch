#pragma once

#include "raylib.h"

class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    bool Init();
    void Unload();
    void PlayHurt() const;

private:
    Sound m_hurtSound{};
    bool m_initialized = false;
};
