#pragma once

#include "ue/core/Types.hpp"

namespace ue::core {

/// Reloj del motor. Proporciona el tiempo transcurrido y los FPS.
/// Se actualiza una vez por fotograma desde el bucle principal.
class Time {
public:
    Time() = default;

    /// Llámese una vez al inicio del fotograma.
    void update(f64 nowSeconds);

    /// Tiempo del último fotograma en segundos.
    f64 deltaSeconds() const { return delta_; }

    /// Tiempo acumulado desde el arranque, en segundos.
    f64 totalSeconds() const { return total_; }

    /// FPS suavizados (media móvil).
    f64 frameRate() const { return fps_; }

    f64 deltaMilliseconds() const { return delta_ * 1000.0; }

private:
    f64 last_   = 0.0;
    f64 delta_  = 0.0;
    f64 total_  = 0.0;
    f64 fps_    = 0.0;
    f64 fpsAccum_ = 0.0;
    i32  fpsFrames_ = 0;
    f64 fpsTimer_  = 0.0;
};

} // namespace ue::core