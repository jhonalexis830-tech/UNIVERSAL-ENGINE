#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Color.hpp"
#include "ue/math/Vector3.hpp"
#include "ue/scene/Component.hpp"

namespace ue::scene {

/// Componente de iluminación.
/// Fase 1: el primer componente de tipo "Sol" existente dirige la luz global.
/// Fase 2+ (roadmap): luces puntuales, focos y sombras dinámicas.
class LightComponent final : public Component {
public:
    enum class Kind {
        Sol,         // luz direccional global
        Punto,       // luz omnidireccional
        Foco,        // spot
    };

    Kind kind = Kind::Sol;
    math::Color color{ 1.0f, 0.98f, 0.94f, 1.0f };
    f32 intensity = 1.0f;
    math::Vector3 direction{ 0.5f, -1.0f, 0.3f }; // solo para Sol/Foco
    math::Vector3 position{ 0.0f, 5.0f, 0.0f };   // solo para Punto/Foco
    f32 range = 20.0f;                            // alcance (Punto/Foco)
    f32 spotAngleDegrees = 45.0f;                 // abertura (Foco)

    bool castsShadows = true;
};

} // namespace ue::scene