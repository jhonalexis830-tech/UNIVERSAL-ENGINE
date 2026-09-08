#include "ue/core/Time.hpp"

namespace ue::core {

void Time::update(f64 nowSeconds) {
    delta_ = (last_ > 0.0) ? (nowSeconds - last_) : 0.0;
    if (delta_ > 0.25) {
        delta_ = 0.25; // evita saltos tras minimizar la ventana
    }
    last_  = nowSeconds;
    total_ += delta_;

    fpsAccum_ += delta_;
    ++fpsFrames_;
    if (fpsAccum_ >= 0.5) {
        fps_ = static_cast<f64>(fpsFrames_) / fpsAccum_;
        fpsAccum_ = 0.0;
        fpsFrames_ = 0;
    }
}

} // namespace ue::core