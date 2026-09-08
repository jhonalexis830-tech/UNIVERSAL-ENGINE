#include "ue/animation/Animator.hpp"

#include "ue/core/Log.hpp"
#include "ue/scene/Scene.hpp"

namespace ue::anim {

void Animator::play(const std::shared_ptr<const AnimationClip>& clip, bool loop) {
    clip_ = clip;
    loop_ = loop;
    time_ = 0.0f;
    playing_ = clip_ != nullptr;
    if (playing_) {
        core::Log::instance().infoFmt("Reproduciendo animación '{}' ({:.2f} s)",
                                      clip_->name, clip_->duration());
    }
}

void Animator::stop() {
    playing_ = false;
    clip_.reset();
    time_ = 0.0f;
}

void Animator::update(f32 deltaSeconds, scene::Scene& scene) {
    if (!playing_ || !clip_) {
        return;
    }
    time_ += deltaSeconds * speed_;

    const f32 duration = clip_->duration();
    if (duration <= 0.0f) {
        return;
    }
    if (time_ > duration) {
        if (loop_) {
            time_ = std::fmod(time_, duration);
        } else {
            time_ = duration;
            playing_ = false;
        }
    }

    Keyframe frame;
    for (const auto& track : clip_->tracks) {
        scene::GameObject* object = scene.find(track.target);
        if (!object) {
            continue;
        }
        if (clip_->evaluate(track.target, time_, frame)) {
            object->transform.position = frame.position;
            object->transform.rotationDegrees = frame.rotationDegrees;
            object->transform.scale = frame.scale;
        }
    }
}

} // namespace ue::anim