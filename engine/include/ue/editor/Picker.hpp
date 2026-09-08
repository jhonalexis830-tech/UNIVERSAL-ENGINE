#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Vector2.hpp"
#include "ue/math/Vector3.hpp"
#include "ue/render/Camera.hpp"
#include "ue/scene/GameObject.hpp"

namespace ue::scene {
class Scene;
class MeshComponent;
}

namespace ue::editor {

/// Rayo desde la cámara hacia un punto de la pantalla.
struct Ray {
    math::Vector3 origin{ 0.0f, 0.0f, 0.0f };
    math::Vector3 direction{ 0.0f, 0.0f, -1.0f };

    bool hitSphere(const math::Vector3& center, f32 radius, f32& tMax) const;
    bool hitAABB(const math::Vector3& minP, const math::Vector3& maxP, f32& tMax) const;
};

/// Utilidades de selección por puntero (ray cast) y edición de vértices.
class Picker {
public:
    /// Construye el rayo desde la cámara a través del píxel dado (en píxeles
    /// lógicos del viewport).
    static Ray cameraRay(const render::Camera& camera, const math::Vector2& mousePx,
                         const math::Vector2& viewportSizePx);

    /// AABBs del mundo de una malla transformada.
    static void worldBounds(const scene::GameObject& object, const scene::MeshComponent& mesh,
                            math::Vector3& outMin, math::Vector3& outMax);

    /// Selecciona el objeto visible más cercano al rayo; devuelve nullptr si no hay.
    static scene::GameObject* pickObject(const scene::Scene& scene, const Ray& ray);
};

} // namespace ue::editor