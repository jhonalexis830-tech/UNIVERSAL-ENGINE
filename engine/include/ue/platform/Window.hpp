#pragma once

#include "ue/platform/IWindow.hpp"

struct GLFWwindow;

namespace ue::platform {

/// Implementación de IWindow usando GLFW.
class Window final : public IWindow {
public:
    Window();
    ~Window() override;

    bool create(i32 width, i32 height, const std::string& title) override;
    void destroy() override;

    bool shouldClose() const override;
    void pollEvents() override;
    void swapBuffers() override;

    void setTitle(const std::string& title) override;

    i32 width() const override        { return width_; }
    i32 height() const override       { return height_; }
    i32 framebufferWidth() const override  { return fbWidth_; }
    i32 framebufferHeight() const override { return fbHeight_; }

    void* nativeHandle() const override;
    void* (*glProcLoader())(const char*) override;

    void setVsync(bool enabled);

private:
    GLFWwindow* window_   = nullptr;
    i32 width_   = 0;
    i32 height_  = 0;
    i32 fbWidth_ = 0;
    i32 fbHeight_= 0;
};

} // namespace ue::platform