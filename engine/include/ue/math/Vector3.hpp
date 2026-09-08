#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Math.hpp"

#include <cmath>
#include <sstream>

namespace ue::math {

/// Vector 3D de propósito general (posición, dirección, escala, color RGB...).
struct Vector3 {
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = 0.0f;

    Vector3() = default;
    Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

    static Vector3 zero()  { return { 0.0f, 0.0f, 0.0f }; }
    static Vector3 one()   { return { 1.0f, 1.0f, 1.0f }; }
    static Vector3 up()    { return { 0.0f, 1.0f, 0.0f }; }
    static Vector3 down()  { return { 0.0f, -1.0f, 0.0f }; }
    static Vector3 right() { return { 1.0f, 0.0f, 0.0f }; }
    static Vector3 left()  { return { -1.0f, 0.0f, 0.0f }; }
    static Vector3 forward(){ return { 0.0f, 0.0f, -1.0f }; }
    static Vector3 back()  { return { 0.0f, 0.0f, 1.0f }; }

    Vector3 operator+(const Vector3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vector3 operator-(const Vector3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vector3 operator-() const { return { -x, -y, -z }; }
    Vector3 operator*(f32 s) const { return { x * s, y * s, z * s }; }
    Vector3 operator*(const Vector3& o) const { return { x * o.x, y * o.y, z * o.z }; }
    Vector3 operator/(f32 s) const { return { x / s, y / s, z / s }; }

    Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vector3& operator*=(f32 s) { x *= s; y *= s; z *= s; return *this; }
    Vector3& operator*=(const Vector3& o) { x *= o.x; y *= o.y; z *= o.z; return *this; }

    bool operator==(const Vector3& o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const Vector3& o) const { return !(*this == o); }

    f32& operator[](i32 i) {
        if (i <= 0) return x;
        if (i == 1) return y;
        return z;
    }
    f32 operator[](i32 i) const {
        if (i <= 0) return x;
        if (i == 1) return y;
        return z;
    }

    f32 dot(const Vector3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vector3 cross(const Vector3& o) const {
        return { y * o.z - z * o.y,
                 z * o.x - x * o.z,
                 x * o.y - y * o.x };
    }
    f32 lengthSq() const { return x * x + y * y + z * z; }
    f32 length() const { return std::sqrt(lengthSq()); }
    Vector3 normalized() const {
        const f32 l = length();
        return (l > 1e-6f) ? Vector3(x / l, y / l, z / l) : Vector3::zero();
    }

    Vector3 lerp(const Vector3& o, f32 t) const {
        return *this + (o - *this) * t;
    }

    /// Rotación por ángulos de Euler en grados (orden X, Y, Z).
    Vector3 rotated(f32 degX, f32 degY, f32 degZ) const {
        const f32 cx = std::cos(degToRad(degX)), sx = std::sin(degToRad(degX));
        const f32 cy = std::cos(degToRad(degY)), sy = std::sin(degToRad(degY));
        const f32 cz = std::cos(degToRad(degZ)), sz = std::sin(degToRad(degZ));
        f32 vx = x, vy = y, vz = z;

        // Rotación en Z
        float tzx = vx * cz - vy * sz;
        float tzy = vx * sz + vy * cz;
        vx = tzx; vy = tzy;
        // Rotación en Y
        float txx = vx * cy + vz * sy;
        float tzz = -vx * sy + vz * cy;
        vx = txx; vz = tzz;
        // Rotación en X
        float tyy = vy * cx - vz * sx;
        float tzz2 = vy * sx + vz * cx;
        vy = tyy; vz = tzz2;
        return { vx, vy, vz };
    }

    std::string toString() const {
        std::ostringstream os;
        os << "(" << x << ", " << y << ", " << z << ")";
        return os.str();
    }
};

} // namespace ue::math