#pragma once

#include "ue/platform/IInput.hpp"

#include <functional>

struct GLFWwindow;

namespace ue::platform {

class Window;

/// Implementación de IInput sobre GLFW mediante callbacks.
///
/// Nota: debe instanciarse ANTES de inicializar Dear ImGui con
/// "install_callbacks = true". ImGui encadena nuestros callbacks y ambos
/// sistemas reciben los mismos eventos sin pisarse.
class Input final : public IInput {
public:
    explicit Input(Window& window);
    ~Input() override;

    void beginFrame(i32 windowWidth, i32 windowHeight) override;

    bool isKeyDown(i32 glfwKeyCode) const override;
    bool isMouseButtonDown(MouseButton button) const override;
    math::Vector2 mousePosition() const override;
    math::Vector2 mouseDelta() const override;
    f32 scrollDelta() const override;

    const std::function<void(i32, i32)>& onFramebufferResized() const override { return onResize_; }
    void setOnFramebufferResized(std::function<void(i32, i32)> callback) override {
        onResize_ = std::move(callback);
    }

    void setCursorCaptured(bool captured) override;

private:
    void installCallbacks(GLFWwindow* window);

    GLFWwindow* window_ = nullptr;
    math::Vector2 mousePos_;
    math::Vector2 mousePrev_;
    math::Vector2 mouseDelta_;
    f32 scrollAccum_   = 0.0f; // acumulado de la rueda
    f32 scrollOfFrame_ = 0.0f; // valor leído al inicio del fotograma
    std::function<void(i32, i32)> onResize_;
};

} // namespace ue::platform