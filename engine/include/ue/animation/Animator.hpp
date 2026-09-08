#pragma once

#include "ue/animation/AnimationClip.hpp"
#include "ue/core/Types.hpp"

#include <memory>

namespace ue::scene {
class Scene;
}

namespace ue::anim {

/// Reproductor de animaciones. Aplica los clips a los GameObjects de la escena
/// cada fotograma (sin acoplar la animación a ningún formato concreto).
class Animator {
public:
    void play(const std::shared_ptr<const AnimationClip>& clip, bool loop = true);
    void stop();
    void pause() { playing_ = false; }
    void resume() { playing_ = true; }

    void setSpeed(f32 speed) { speed_ = speed; }
    f32 speed() const { return speed_; }

    /// Si reproduce, avanza el reloj y aplica las pistas a la escena.
    void update(f32 deltaSeconds, scene::Scene& scene);

    bool isPlaying() const { return playing_ && clip_ != nullptr; }
    f32 currentTime() const { return time_; }

private:
    std::shared_ptr<const AnimationClip> clip_;
    bool playing_ = false;
    bool loop_ = true;
    f32 time_ = 0.0f;
    f32 speed_ = 1.0f;
};

} // namespace ue::anim