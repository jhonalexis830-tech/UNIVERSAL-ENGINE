#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Vector2.hpp"

#include <functional>

namespace ue::platform {

/// Estados de los botones del ratón.
enum class MouseButton : i32 {
    Left   = 0,
    Right  = 1,
    Middle = 2,
};

/// Eventos reemitidos por el gestor de entrada.
struct KeyEvent {
    i32 key = 0;        // código GLFW
    i32 action = 0;     // GLFW_PRESS / GLFW_RELEASE / GLFW_REPEAT
    i32 mods = 0;
    bool consumed = false;
};

/// Contrato del gestor de entrada (SRP: lectura de teclado y ratón en un solo lugar).
class IInput {
public:
    virtual ~IInput() = default;

    /// Actualiza el estado interno una vez por fotograma.
    virtual void beginFrame(i32 windowWidth, i32 windowHeight) = 0;

    virtual bool isKeyDown(i32 glfwKeyCode) const = 0;
    virtual bool isMouseButtonDown(MouseButton button) const = 0;

    /// Posición actual del cursor en píxeles de la ventana (origen arriba-izquierda).
    virtual math::Vector2 mousePosition() const = 0;

    /// Desplazamiento del cursor respecto al fotograma anterior.
    virtual math::Vector2 mouseDelta() const = 0;

    /// Acumulado del desplazamiento de la rueda del ratón.
    virtual f32 scrollDelta() const = 0;

    /// Tamaño del viewport repintado por el SO (redimensionado de la ventana).
    virtual const std::function<void(i32, i32)>& onFramebufferResized() const = 0;
    virtual void setOnFramebufferResized(std::function<void(i32, i32)> callback) = 0;

    /// Ocultar y fijar el cursor (modo vuelo / primera persona).
    virtual void setCursorCaptured(bool captured) = 0;
};

} // namespace ue::platform