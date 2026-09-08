#pragma once

#include "ue/core/Types.hpp"

#include <string>

namespace ue::platform {

/// Abstracción de la ventana del sistema (SRP: la ventana solo gestiona su ciclo de vida).
class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool create(i32 width, i32 height, const std::string& title) = 0;
    virtual void destroy() = 0;

    virtual bool shouldClose() const = 0;
    virtual void pollEvents() = 0;
    virtual void swapBuffers() = 0;

    virtual void setTitle(const std::string& title) = 0;

    virtual i32 width() const = 0;
    virtual i32 height() const = 0;
    virtual i32 framebufferWidth() const = 0;
    virtual i32 framebufferHeight() const = 0;

    /// Puntero nativo (GLFWwindow* en Windows).
    virtual void* nativeHandle() const = 0;

    /// Cargador de procedimientos OpenGL de esta ventana (para GlLoader).
    virtual void*(*glProcLoader())(const char*) = 0;
};

} // namespace ue::platform