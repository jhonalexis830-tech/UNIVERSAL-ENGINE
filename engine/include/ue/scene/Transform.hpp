#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Matrix4.hpp"
#include "ue/math/Vector3.hpp"

namespace ue::scene {

/// Transformación local de un GameObject (SRP: posición/rotación/escala + matriz).
class Transform {
public:
    math::Vector3 position{ 0.0f, 0.0f, 0.0f };
    math::Vector3 rotationDegrees{ 0.0f, 0.0f, 0.0f };
    math::Vector3 scale{ 1.0f, 1.0f, 1.0f };

    /// Matriz modelo TRS (columna-mayor, lista para OpenGL).
    math::Matrix4 matrix() const {
        return math::Matrix4::compose(position, rotationDegrees, scale);
    }

    void setPosition(f32 x, f32 y, f32 z) { position = { x, y, z }; }
    void setScale(f32 s) { scale = { s, s, s }; }
};

} // namespace ue::scene