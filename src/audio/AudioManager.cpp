#include "AudioManager.h"
#include "io/AssetPaths.h"

AudioManager::~AudioManager() {
    Unload();
}

bool AudioManager::Init() {
    InitAudioDevice();
    m_hurtSound = LoadSound(AssetPaths::HURT_SOUND);
    if (m_hurtSound.frameCount == 0) {
        TraceLog(LOG_WARNING, "AUDIO: Failed to load hurt sound: %s", AssetPaths::HURT_SOUND);
        return false;
    }
    m_initialized = true;
    return true;
}

void AudioManager::Unload() {
    if (m_initialized) {
        UnloadSound(m_hurtSound);
        CloseAudioDevice();
        m_initialized = false;
    }
}

void AudioManager::PlayHurt() const {
    if (m_initialized && m_hurtSound.frameCount > 0) {
        PlaySound(m_hurtSound);
    }
}
