#pragma once

#include "ue/core/Types.hpp"

#include <algorithm>
#include <cmath>

namespace ue::math {

constexpr f32 kPi     = 3.14159265358979323846f;
constexpr f32 kPi2    = kPi * 2.0f;
constexpr f32 kDegToRad = kPi / 180.0f;
constexpr f32 kRadToDeg = 180.0f / kPi;

inline f32 degToRad(f32 degrees) { return degrees * kDegToRad; }
inline f32 radToDeg(f32 radians) { return radians * kRadToDeg; }
inline f32 clamp01(f32 v) { return std::clamp(v, 0.0f, 1.0f); }

} // namespace ue::math