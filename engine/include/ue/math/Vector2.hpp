#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Math.hpp"

#include <cmath>
#include <sstream>

namespace ue::math {

/// Vector 2D de propósito general.
struct Vector2 {
    f32 x = 0.0f;
    f32 y = 0.0f;

    Vector2() = default;
    Vector2(f32 x, f32 y) : x(x), y(y) {}

    static Vector2 zero() { return { 0.0f, 0.0f }; }

    Vector2 operator+(const Vector2& o) const { return { x + o.x, y + o.y }; }
    Vector2 operator-(const Vector2& o) const { return { x - o.x, y - o.y }; }
    Vector2 operator*(f32 s) const { return { x * s, y * s }; }
    Vector2 operator/(f32 s) const { return { x / s, y / s }; }
    Vector2& operator+=(const Vector2& o) { x += o.x; y += o.y; return *this; }
    Vector2& operator-=(const Vector2& o) { x -= o.x; y -= o.y; return *this; }
    Vector2& operator*=(f32 s) { x *= s; y *= s; return *this; }

    f32 dot(const Vector2& o) const { return x * o.x + y * o.y; }
    f32 lengthSq() const { return x * x + y * y; }
    f32 length() const { return std::sqrt(lengthSq()); }
    Vector2 normalized() const {
        const f32 l = length();
        return (l > 1e-6f) ? Vector2(x / l, y / l) : Vector2(0.0f, 0.0f);
    }

    std::string toString() const {
        std::ostringstream os;
        os << "(" << x << ", " << y << ")";
        return os.str();
    }
};

} // namespace ue::math