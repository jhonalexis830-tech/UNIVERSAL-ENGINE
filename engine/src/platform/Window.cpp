#include "ue/platform/Window.hpp"

#include <GLFW/glfw3.h>

namespace ue::platform {

namespace {
    void glfwErrorCallback(int error, const char* description) {
        // El registro aún no está disponible durante create(); se imprime directo.
        std::fprintf(stderr, "[GLFW] error %d: %s\n", error, description ? description : "");
    }
}

Window::Window() = default;

Window::~Window() {
    destroy();
}

bool Window::create(i32 width, i32 height, const std::string& title) {
    if (!glfwInit()) {
        return false;
    }
    glfwSetErrorCallback(glfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4); // antialias

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        return false;
    }

    width_  = width;
    height_ = height;
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    glfwGetFramebufferSize(window_, &fbWidth_, &fbHeight_);
    return true;
}

void Window::destroy() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return window_ ? (glfwWindowShouldClose(window_) != 0) : true;
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    if (window_) {
        glfwSwapBuffers(window_);
    }
}

void Window::setTitle(const std::string& title) {
    if (window_) {
        glfwSetWindowTitle(window_, title.c_str());
    }
}

void* Window::nativeHandle() const {
    return window_;
}

void* (*Window::glProcLoader())(const char*) {
    return [](const char* name) -> void* {
        return reinterpret_cast<void*>(glfwGetProcAddress(name));
    };
}

void Window::setVsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

} // namespace ue::platform