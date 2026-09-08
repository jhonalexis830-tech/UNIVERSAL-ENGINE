#include "ue/platform/Input.hpp"

#include "ue/platform/Window.hpp"

#include <GLFW/glfw3.h>

namespace ue::platform {

namespace {
    // Hay una única instancia de Input en la aplicación. GLFW sobrescribe el
    // user pointer cuando ImGui se inicializa con install_callbacks=true, por
    // lo que este puntero estático es la vía fiable de despacho (patrón usado
    // también por el backend de ImGui para GLFW).
    Input* gCurrentInput = nullptr;
}

Input::Input(Window& window) {
    window_ = static_cast<GLFWwindow*>(window.nativeHandle());
    gCurrentInput = this;
    installCallbacks(window_);
}

Input::~Input() {
    if (gCurrentInput == this) {
        gCurrentInput = nullptr;
    }
}

void Input::installCallbacks(GLFWwindow* window) {
    glfwSetScrollCallback(window, [](GLFWwindow*, double, double yoffset) {
        if (gCurrentInput) {
            gCurrentInput->scrollAccum_ += static_cast<f32>(yoffset);
        }
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
        if (gCurrentInput && gCurrentInput->onResize_) {
            gCurrentInput->onResize_(width, height);
        }
    });
}

void Input::beginFrame(i32 windowWidth, i32 windowHeight) {
    (void)windowWidth; (void)windowHeight;
    if (!window_) {
        return;
    }

    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    mousePrev_  = mousePos_;
    mousePos_   = math::Vector2(static_cast<f32>(x), static_cast<f32>(y));
    mouseDelta_ = mousePos_ - mousePrev_;

    // La rueda se acumula en el callback durante pollEvents().
    scrollOfFrame_ = scrollAccum_;
    scrollAccum_   = 0.0f;
}

bool Input::isKeyDown(i32 glfwKeyCode) const {
    if (!window_) {
        return false;
    }
    return glfwGetKey(window_, glfwKeyCode) == GLFW_PRESS;
}

bool Input::isMouseButtonDown(MouseButton button) const {
    if (!window_) {
        return false;
    }
    return glfwGetMouseButton(window_, static_cast<i32>(button)) == GLFW_PRESS;
}

math::Vector2 Input::mousePosition() const {
    return mousePos_;
}

math::Vector2 Input::mouseDelta() const {
    return mouseDelta_;
}

f32 Input::scrollDelta() const {
    return scrollOfFrame_;
}

void Input::setCursorCaptured(bool captured) {
    if (window_) {
        glfwSetInputMode(window_, GLFW_CURSOR,
                         captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

} // namespace ue::platform