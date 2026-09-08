#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Matrix4.hpp"
#include "ue/math/Vector3.hpp"

namespace ue::render {

/// Datos de iluminación global de la escena (Fase 1: un sol direccional + ambiente).
struct Lights {
    math::Vector3 sunDirection{ 0.5f, -1.0f, 0.3f };
    math::Vector3 sunColor{ 1.0f, 0.98f, 0.94f };
    math::Vector3 ambient{ 0.16f, 0.18f, 0.24f };

    static Lights defaultLights() { return {}; }
};

/// Cámara virtual para renderizar, grabar y tomar fotos.
/// Se posiciona mediante target + azimut/elevación/distancia (estilo Blender).
class Camera {
public:
    math::Vector3 position{ 8.0f, 6.0f, 10.0f };
    math::Vector3 target{ 0.0f, 0.6f, 0.0f };
    math::Vector3 up{ 0.0f, 1.0f, 0.0f };

    f32 fovYDegrees = 60.0f;
    f32 nearPlane = 0.05f;
    f32 farPlane  = 2000.0f;

    /// Matriz de vista (lookAt).
    math::Matrix4 viewMatrix() const;

    /// Matriz de proyección en perspectiva.
    math::Matrix4 projectionMatrix(f32 aspectRatio) const;

    /// Vectores ortonormales de la cámara.
    math::Vector3 forward() const;
    math::Vector3 right() const;
    math::Vector3 upVector() const;

    void lookAt(const math::Vector3& pos, const math::Vector3& tgt) {
        position = pos;
        target = tgt;
    }
};

} // namespace ue::render