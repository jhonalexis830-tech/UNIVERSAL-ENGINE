#pragma once

#include "ue/core/Types.hpp"
#include "ue/core/Uuid.hpp"
#include "ue/math/Vector3.hpp"

#include <string>
#include <vector>

namespace ue::anim {

/// Fotograma clave de una pista (posición, rotación y escala).
struct Keyframe {
    f32 time = 0.0f;
    math::Vector3 position{ 0.0f, 0.0f, 0.0f };
    math::Vector3 rotationDegrees{ 0.0f, 0.0f, 0.0f };
    math::Vector3 scale{ 1.0f, 1.0f, 1.0f };
};

/// Pista de animación asociada a un GameObject (por UUID).
struct Track {
    core::Uuid target;
    std::vector<Keyframe> frames;
};

/// Clip de animación programable: lista de pistas con fotogramas clave.
/// Las animaciones pueden definirse con código o grabarse con el auto-key.
class AnimationClip {
public:
    std::string name = "Animación";
    std::vector<Track> tracks;

    Track& addTrack(const core::Uuid& targetId);
    void addKeyframe(const core::Uuid& targetId, const Keyframe& frame);

    /// Ajusta la duración al último fotograma y ordena cada pista por tiempo.
    void finalize(f32 durationHint = -1.0f);

    f32 duration() const { return duration_; }

    /// Evalúa la pista en `time` interpolando linealmente los fotogramas.
    /// Devuelve false si no existe pista para el objetivo.
    bool evaluate(const core::Uuid& targetId, f32 time, Keyframe& out) const;

private:
    f32 duration_ = 0.0f;
};

} // namespace ue::anim