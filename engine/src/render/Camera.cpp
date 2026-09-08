#include "ue/render/Camera.hpp"

namespace ue::render {

math::Matrix4 Camera::viewMatrix() const {
    return math::Matrix4::lookAt(position, target, up);
}

math::Matrix4 Camera::projectionMatrix(f32 aspectRatio) const {
    return math::Matrix4::perspective(fovYDegrees, aspectRatio, nearPlane, farPlane);
}

math::Vector3 Camera::forward() const {
    return (target - position).normalized();
}

math::Vector3 Camera::right() const {
    return forward().cross(up).normalized();
}

math::Vector3 Camera::upVector() const {
    return right().cross(forward()).normalized();
}

} // namespace ue::render