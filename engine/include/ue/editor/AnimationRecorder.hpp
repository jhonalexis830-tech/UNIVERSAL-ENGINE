#pragma once

#include "ue/animation/AnimationClip.hpp"
#include "ue/core/Types.hpp"

namespace ue::scene {
class GameObject;
class Scene;
}

namespace ue::editor {

/// Graba transformaciones de un GameObject como fotogramas clave:
/// si el transform cambia respecto al último clave, inserta un Keyframe.
class AnimationRecorder {
public:
    void start(anim::AnimationClip& clip, ue::scene::GameObject& target);
    void stop();
    void recordFrame(const ue::scene::Scene& scene, f32 timeSeconds);

    bool isRecording() const { return clip_ != nullptr; }
    anim::AnimationClip* clip() const { return clip_; }

private:
    anim::AnimationClip* clip_ = nullptr;
    core::Uuid targetId_;
    f32 lastTime_ = 0.0f;
    bool hasLast_ = false;
    ue::math::Vector3 lastKeyState_{ 1e9f, 1e9f, 1e9f };
};

} // namespace ue::editor