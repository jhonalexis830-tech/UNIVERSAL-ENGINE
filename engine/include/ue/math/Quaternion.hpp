#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Math.hpp"
#include "ue/math/Vector3.hpp"

namespace ue::math {

class Matrix4;

/// Cuaternión unitario (w + xi + yj + zk).
class Quaternion {
public:
    f32 w = 1.0f;
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = 0.0f;

    Quaternion() = default;
    Quaternion(f32 w, f32 x, f32 y, f32 z) : w(w), x(x), y(y), z(z) {}

    static Quaternion identity() { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }

    /// Construye a partir de un eje y un ángulo en grados.
    static Quaternion fromAxisAngle(const Vector3& axis, f32 degrees);

    /// Construye desde ángulos de Euler en grados (orden X, Y, Z).
    static Quaternion fromEulerDeg(f32 degX, f32 degY, f32 degZ);

    Quaternion normalized() const;
    Quaternion conjugate() const;

    /// Multiplicación de cuaterniones (composición de rotaciones).
    Quaternion operator*(const Quaternion& o) const;

    Vector3 rotate(const Vector3& v) const;

    /// Interpolación lineal normalizada; t en [0,1].
    static Quaternion nlerp(const Quaternion& a, const Quaternion& b, f32 t);
};

} // namespace ue::math