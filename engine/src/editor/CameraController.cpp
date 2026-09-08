#include "ue/editor/CameraController.hpp"

#include "ue/core/Log.hpp"
#include "ue/math/Math.hpp"
#include "ue/platform/IInput.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

namespace ue::editor {

using math::Vector3;

CameraController::CameraController(render::Camera& camera)
    : camera_(camera) {
    // Deduce la órbita actual de la cámara.
    const Vector3 offset = camera_.position - camera_.target;
    distance_ = std::max(offset.length(), 0.01f);
    azimuth_ = math::radToDeg(std::atan2(offset.z, offset.x));
    elevation_ = math::radToDeg(std::asin(std::clamp(offset.y / distance_, -1.0f, 1.0f)));
}

void CameraController::syncOrbital() {
    const f32 az = math::degToRad(azimuth_);
    const f32 el = math::degToRad(elevation_);
    const Vector3 offset(std::cos(az) * std::cos(el),
                         std::sin(el),
                         std::sin(az) * std::cos(el));
    camera_.position = target_ + offset * distance_;
    camera_.target = target_;
    camera_.lookAt(camera_.position, target_);
}

void CameraController::update(platform::IInput& input, f32 deltaSeconds, bool viewportActs) {
    if (!viewportActs) {
        syncOrbital();
        return;
    }

    const bool leftShift = input.isKeyDown(GLFW_KEY_LEFT_SHIFT);
    const bool middle = input.isMouseButtonDown(platform::MouseButton::Middle);

    if (flying_) {
        // Modo vuelo: mirar con el ratón, moverse con WASD/E/Q.
        if (!flyInitialized_) {
            const Vector3 fwd = camera_.forward().normalized();
            flyYaw_ = std::atan2(fwd.z, fwd.x);
            flyPitch_ = std::asin(std::clamp(fwd.y, -1.0f, 1.0f));
            flyInitialized_ = true;
        }
        const Vector2 delta = input.mouseDelta();
        flyYaw_ += delta.x * 0.0024f;
        flyPitch_ = std::clamp(flyPitch_ - delta.y * 0.0024f, -1.53f, 1.53f);

        const Vector3 dir = Vector3(std::cos(flyPitch_) * std::cos(flyYaw_),
                                    std::sin(flyPitch_),
                                    std::cos(flyPitch_) * std::sin(flyYaw_))
                                .normalized();
        const Vector3 worldUp = Vector3::up();
        const Vector3 right = dir.cross(worldUp).normalized();
        const Vector3 up = right.cross(dir).normalized();

        const f32 amount = deltaSeconds * (leftShift ? 18.0f : 6.0f);
        Vector3 move(0.0f, 0.0f, 0.0f);
        if (input.isKeyDown(GLFW_KEY_W)) move += dir;
        if (input.isKeyDown(GLFW_KEY_S)) move -= dir;
        if (input.isKeyDown(GLFW_KEY_D)) move += right;
        if (input.isKeyDown(GLFW_KEY_A)) move -= right;
        if (input.isKeyDown(GLFW_KEY_E)) move += up;
        if (input.isKeyDown(GLFW_KEY_Q)) move -= up;

        const Vector3 dv = move * amount;
        camera_.position += dv;
        target_ = camera_.position + dir;
        camera_.target = target_;
        camera_.lookAt(camera_.position, target_);
        return;
    }

    // Al salir del modo vuelo se recalculan los ángulos orbitales.
    if (flyInitialized_) {
        const Vector3 offset = camera_.position - target_;
        distance_ = std::max(offset.length(), 0.01f);
        azimuth_ = math::radToDeg(std::atan2(offset.z, offset.x));
        elevation_ = math::radToDeg(std::asin(std::clamp(offset.y / distance_, -1.0f, 1.0f)));
        flyInitialized_ = false;
    }

    if (middle) {
        const Vector2 delta = input.mouseDelta();
        if (leftShift) {
            // Desplazamiento: Mueve el objetivo con el plano de la cámara.
            const f32 scale = distance_ * 0.0012f;
            const Vector3 right = camera_.right();
            const Vector3 up = camera_.upVector();
            target_ += right * (-delta.x * scale) + up * (delta.y * scale);
        } else {
            // Órbita
            azimuth_ -= delta.x * 0.30f;
            elevation_ = std::clamp(elevation_ + delta.y * 0.30f, -89.0f, 89.0f);
        }
    }

    // Zoom con la rueda.
    const f32 scroll = input.scrollDelta();
    if (scroll != 0.0f) {
        distance_ *= std::exp(-scroll * 0.12f);
        distance_ = std::clamp(distance_, 0.2f, 800.0f);
    }

    // Vistas rápidas (como Blender): 7 superior, 1 frente, 3 lateral.
    if (input.isKeyDown(GLFW_KEY_7)) {
        azimuth_ = 0.0f;
        elevation_ = 90.0f;
    } else if (input.isKeyDown(GLFW_KEY_1)) {
        azimuth_ = 90.0f;
        elevation_ = 0.0f;
    } else if (input.isKeyDown(GLFW_KEY_3)) {
        azimuth_ = 0.0f;
        elevation_ = 0.0f;
    }

    syncOrbital();
}

void CameraController::focusOn(const math::Vector3& worldPoint, f32 focusDistance) {
    target_ = worldPoint;
    distance_ = std::clamp(focusDistance, 0.5f, 400.0f);
    syncOrbital();
}

void CameraController::centerOnTarget() const {
    camera_.target = target_;
}

} // namespace ue::editor