#pragma once

#include "ue/core/Types.hpp"

namespace ue::math {

/// Color RGBA en coma flotante [0,1].
struct Color {
    f32 r = 1.0f;
    f32 g = 1.0f;
    f32 b = 1.0f;
    f32 a = 1.0f;

    Color() = default;
    Color(f32 r, f32 g, f32 b, f32 a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static Color black()      { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
    static Color white()      { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
    static Color red()        { return { 0.86f, 0.29f, 0.29f, 1.0f }; }
    static Color green()      { return { 0.36f, 0.78f, 0.36f, 1.0f }; }
    static Color blue()       { return { 0.33f, 0.64f, 0.89f, 1.0f }; }
    static Color metallicBlue(){ return { 0.29f, 0.50f, 0.84f, 1.0f }; }
    static Color deepBlack()  { return { 0.035f, 0.043f, 0.063f, 1.0f }; }
    static Color gray()       { return { 0.45f, 0.48f, 0.54f, 1.0f }; }
};

} // namespace ue::math