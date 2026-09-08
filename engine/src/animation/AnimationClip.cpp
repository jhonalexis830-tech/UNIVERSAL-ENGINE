#include "ue/animation/AnimationClip.hpp"

#include <algorithm>

namespace ue::anim {

Track& AnimationClip::addTrack(const core::Uuid& targetId) {
    for (auto& track : tracks) {
        if (track.target == targetId) {
            return track;
        }
    }
    tracks.push_back(Track{ targetId, {} });
    return tracks.back();
}

void AnimationClip::addKeyframe(const core::Uuid& targetId, const Keyframe& frame) {
    Track& track = addTrack(targetId);
    track.frames.push_back(frame);
}

void AnimationClip::finalize(f32 durationHint) {
    for (auto& track : tracks) {
        std::sort(track.frames.begin(), track.frames.end(),
                  [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
        if (!track.frames.empty()) {
            duration_ = std::max(duration_, track.frames.back().time);
        }
    }
    if (durationHint >= 0.0f) {
        duration_ = std::max(duration_, durationHint);
    }
}

bool AnimationClip::evaluate(const core::Uuid& targetId, f32 time, Keyframe& out) const {
    for (const auto& track : tracks) {
        if (track.target != targetId) {
            continue;
        }
        const auto& frames = track.frames;
        if (frames.empty()) {
            return false;
        }
        if (frames.size() == 1 || time <= frames.front().time) {
            out = frames.front();
            return true;
        }
        if (time >= frames.back().time) {
            out = frames.back();
            return true;
        }
        // Búsqueda del par [a, b] que rodea a time.
        size_t b = 1;
        while (b < frames.size() && frames[b].time < time) {
            ++b;
        }
        const Keyframe& fa = frames[b - 1];
        const Keyframe& fb = frames[b];
        const f32 t = (time - fa.time) / std::max(fb.time - fa.time, 1e-6f);
        out.time = time;
        out.position = fa.position.lerp(fb.position, t);
        out.rotationDegrees = fa.rotationDegrees.lerp(fb.rotationDegrees, t);
        out.scale = fa.scale.lerp(fb.scale, t);
        return true;
    }
    return false;
}

} // namespace ue::anim