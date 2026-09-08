#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Vector3.hpp"
#include "ue/scene/GameObject.hpp"

#include <map>
#include <memory>

namespace ue::scene {
class Scene;
}

namespace ue::physics {

/// Cuerpo rígido simple (Fase 1: gravedad + rebote contra planos).
class RigidBody {
public:
    math::Vector3 velocity{ 0.0f, 0.0f, 0.0f };
    f32 mass = 1.0f;
    bool dynamic = true;        // false = rígido (colisiona pero no se mueve)
    f32 restitution = 0.68f;    // rebote
    f32 friction = 0.2f;        // frenado horizontal
    f32 radius = 0.0f;          // radio de colisión esférica (0 = ninguno)
};

/// Motor de física mínimo. Integra la gravedad y rebota contra el suelo.
/// En fases posteriores se ampliará con hitboxes, fuerzas, y simuladores.
class PhysicsWorld {
public:
    void setGravity(const math::Vector3& g) { gravity_ = g; }

    /// Registra un objeto con cuerpo rígido (por referencia al objeto).
    void registerBody(const std::shared_ptr<scene::GameObject>& object, f32 radius = 0.0f);

    void unregister(scene::GameObject* object);

    /// Avanza la simulación aplicando las posiciones a la escena.
    void update(f32 deltaSeconds, scene::Scene& scene);

private:
    struct Body {
        std::shared_ptr<scene::GameObject> object;
        RigidBody rb;
    };

    math::Vector3 gravity_{ 0.0f, -9.8f, 0.0f };
    std::vector<std::unique_ptr<Body>> bodies_;
};

} // namespace ue::physics