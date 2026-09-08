#include "ue/editor/AnimationRecorder.hpp"

#include "ue/math/Vector3.hpp"
#include "ue/scene/GameObject.hpp"
#include "ue/scene/Scene.hpp"

namespace ue::editor {

using ue::math::Vector3;

void AnimationRecorder::start(anim::AnimationClip& clip, ue::scene::GameObject& target) {
    clip_ = &clip;
    targetId_ = target.id;
    hasLast_ = false;
    lastKeyState_ = Vector3(1e9f, 1e9f, 1e9f);
    lastTime_ = 0.0f;
}

void AnimationRecorder::stop() {
    clip_ = nullptr;
    hasLast_ = false;
}

void AnimationRecorder::recordFrame(const ue::scene::Scene& scene, f32 timeSeconds) {
    if (!clip_ || timeSeconds < lastTime_) {
        if (timeSeconds < lastTime_) {
            // El reloj se reinició: se reinicia el seguimiento.
            hasLast_ = false;
        }
        return;
    }

    ue::scene::GameObject* targetG = nullptr;
    for (auto* object : scene.allObjects()) {
        if (object->id == targetId_) {
            targetG = object;
            break;
        }
    }
    if (!targetG) {
        return;
    }

    const Vector3 state(targetG->transform.position.x, targetG->transform.position.y,
                        targetG->transform.position.z);
    const bool same = (state - lastKeyState_).lengthSq() < 1e-8f;
    if (!hasLast_ || !same) {
        anim::Keyframe frame;
        frame.time = timeSeconds;
        frame.position = targetG->transform.position;
        frame.rotationDegrees = targetG->transform.rotationDegrees;
        frame.scale = targetG->transform.scale;
        clip_->addKeyframe(targetId_, frame);
        lastKeyState_ = state;
        hasLast_ = true;
        lastTime_ = timeSeconds;
    }
}

} // namespace ue::editor