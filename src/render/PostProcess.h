#pragma once

#include "raylib.h"

class PostProcess {
public:
    PostProcess() = default;
    ~PostProcess();

    PostProcess(const PostProcess&) = delete;
    PostProcess& operator=(const PostProcess&) = delete;
    PostProcess(PostProcess&&) = default;
    PostProcess& operator=(PostProcess&&) = default;

    bool Init(const char* fragmentShaderPath);
    void Unload();
    void UpdateSize(int width, int height);

    void BeginCapture();
    void EndCapture();
    void Render() const;

    [[nodiscard]] bool IsInitialized() const noexcept { return m_initialized; }

    bool enabled = true;
    int mode = 0;
    Vector2 playerPos{ 0.0f, 0.0f };
    float powerMode = 0.0f;

private:
    RenderTexture2D m_target{};
    Shader m_shader{};
    int m_locResolution = -1;
    int m_locTime = -1;
    int m_locMode = -1;
    int m_locPlayerPos = -1;
    int m_locPower = -1;
    bool m_initialized = false;
};
