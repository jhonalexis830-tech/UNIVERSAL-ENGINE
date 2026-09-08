#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Vector3.hpp"
#include "ue/render/Camera.hpp"

#include <functional>

namespace ue::platform {
class IInput;
}

namespace ue::editor {

/// Control de la cámara estilo Blender:
///  - Rueda del ratón ......... zoom
///  - Botón central ........... órbita
///  - MAYÚS + botón central ... desplazamiento (pan)
///  - Teclas 7 / 1 / 3 ........ vistas superior / frente / lateral
///  - F ....................... modo vuelo (WASD + E/Q + ratón)
///  - Espacio ................. centrar en el objetivo
class CameraController {
public:
    explicit CameraController(render::Camera& camera);

    /// Actualiza la cámara usando el estado de entrada.
    void update(platform::IInput& input, f32 deltaSeconds, bool viewportActs);

    void focusOn(const math::Vector3& worldPoint, f32 distance = 6.0f);
    void centerOnTarget() const;

    bool isFlying() const { return flying_; }
    void setFlying(bool value) { flying_ = value; }

    f32 azimuthDeg() const { return azimuth_; }
    f32 elevationDeg() const { return elevation_; }
    f32 distance() const { return distance_; }

private:
    void syncOrbital();

    render::Camera& camera_;
    math::Vector3 target_{ 0.0f, 0.9f, 0.0f };
    f32 azimuth_ = 45.0f;    // grados
    f32 elevation_ = 28.0f;  // grados
    f32 distance_ = 12.0f;
    bool flying_ = false;
    bool flyInitialized_ = false;
    f32 flyYaw_ = 0.0f;
    f32 flyPitch_ = 0.0f;
};

} // namespace ue::editor