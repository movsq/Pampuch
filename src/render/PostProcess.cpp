#include "PostProcess.h"

PostProcess::~PostProcess() {
    Unload();
}

bool PostProcess::Init(const char* fragmentShaderPath) {
    m_shader = LoadShader(nullptr, fragmentShaderPath);
    if (m_shader.id == 0) {
        TraceLog(LOG_WARNING, "POST_PROCESS: Failed to load shader: %s", fragmentShaderPath);
        return false;
    }

    m_locResolution = GetShaderLocation(m_shader, "resolution");
    m_locTime       = GetShaderLocation(m_shader, "time");
    m_locMode       = GetShaderLocation(m_shader, "mode");
    m_locPlayerPos  = GetShaderLocation(m_shader, "playerPos");
    m_locPower      = GetShaderLocation(m_shader, "powerMode");

    m_target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    if (m_target.id == 0) {
        TraceLog(LOG_WARNING, "POST_PROCESS: Failed to create initial render texture");
        return false;
    }

    m_initialized = true;
    return true;
}

void PostProcess::Unload() {
    if (m_initialized) {
        UnloadShader(m_shader);
        UnloadRenderTexture(m_target);
        m_initialized = false;
    }
}

void PostProcess::UpdateSize(int width, int height) {
    if (m_target.id > 0) UnloadRenderTexture(m_target);
    m_target = LoadRenderTexture(width, height);
    if (m_target.id == 0) {
        TraceLog(LOG_WARNING, "POST_PROCESS: Failed to recreate render texture");
    }
}

void PostProcess::BeginCapture() {
    BeginTextureMode(m_target);
    ClearBackground(BLACK);
}

void PostProcess::EndCapture() {
    EndTextureMode();
}

void PostProcess::Render() const {
    const float sw = static_cast<float>(GetScreenWidth());
    const float sh = static_cast<float>(GetScreenHeight());

    const Rectangle sourceRect = {
        0.0f, 0.0f,
        static_cast<float>(m_target.texture.width),
        static_cast<float>(-m_target.texture.height)
    };
    const Rectangle destRect = { 0.0f, 0.0f, sw, sh };
    const Vector2 origin = { 0.0f, 0.0f };

    if (enabled && m_initialized) {
        float resolutions[2] = { sw, sh };
        SetShaderValue(m_shader, m_locResolution, resolutions, SHADER_UNIFORM_VEC2);

        float time = static_cast<float>(GetTime());
        SetShaderValue(m_shader, m_locTime, &time, SHADER_UNIFORM_FLOAT);

        SetShaderValue(m_shader, m_locMode, &mode, SHADER_UNIFORM_INT);

        float player[2] = { playerPos.x, playerPos.y };
        SetShaderValue(m_shader, m_locPlayerPos, player, SHADER_UNIFORM_VEC2);

        SetShaderValue(m_shader, m_locPower, &powerMode, SHADER_UNIFORM_FLOAT);

        BeginShaderMode(m_shader);
        DrawTexturePro(m_target.texture, sourceRect, destRect, origin, 0.0f, WHITE);
        EndShaderMode();
    } else {
        DrawTexturePro(m_target.texture, sourceRect, destRect, origin, 0.0f, WHITE);
    }
}
