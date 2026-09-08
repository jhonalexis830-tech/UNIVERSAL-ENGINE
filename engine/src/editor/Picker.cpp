#include "ue/editor/Picker.hpp"

#include "ue/math/Math.hpp"
#include "ue/render/Vertex.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <algorithm>
#include <cmath>

namespace ue::editor {

namespace {
    f32 min3(f32 a, f32 b, f32 c) { return std::min(std::min(a, b), c); }
    f32 max3(f32 a, f32 b, f32 c) { return std::max(std::max(a, b), c); }
}

bool Ray::hitSphere(const math::Vector3& center, f32 radius, f32& tMax) const {
    const math::Vector3 toCenter = center - origin;
    const f32 proj = toCenter.dot(direction);
    if (proj < 0.0f) {
        return false;
    }
    const f32 perpendicular = toCenter.lengthSq() - proj * proj;
    if (perpendicular > radius * radius) {
        return false;
    }
    const f32 inside = std::sqrt(std::max(radius * radius - perpendicular, 0.0f));
    const f32 t = proj - inside;
    if (t < 0.0f || t > tMax) {
        return false;
    }
    tMax = t;
    return true;
}

bool Ray::hitAABB(const math::Vector3& minP, const math::Vector3& maxP, f32& tMax) const {
    float tmin = 0.0f;
    float tmax = tMax;
    for (int axis = 0; axis < 3; ++axis) {
        const f32 o = origin[axis];
        const f32 d = direction[axis];
        f32 to1, to2;
        if (std::fabs(d) < 1e-8f) {
            if (o < minP[axis] || o > maxP[axis]) {
                return false;
            }
            continue;
        }
        to1 = (minP[axis] - o) / d;
        to2 = (maxP[axis] - o) / d;
        if (to1 > to2) std::swap(to1, to2);
        tmin = std::max(tmin, to1);
        tmax = std::min(tmax, to2);
        if (tmin > tmax) {
            return false;
        }
    }
    if (tmin > tMax) {
        return false;
    }
    tMax = tmin;
    return true;
}

Ray Picker::cameraRay(const render::Camera& camera, const math::Vector2& mousePx,
                      const math::Vector2& viewportSizePx) {
    const f32 w = std::max(viewportSizePx.x, 1.0f);
    const f32 h = std::max(viewportSizePx.y, 1.0f);

    const f32 tanHalfY = std::tan(math::degToRad(camera.fovYDegrees) * 0.5f);
    const f32 aspect = w / h;
    const f32 ndcX = (mousePx.x / w) * 2.0f - 1.0f;
    const f32 ndcY = 1.0f - (mousePx.y / h) * 2.0f; // origen arriba-izquierda

    const math::Vector3 right = camera.right();
    const math::Vector3 up = camera.upVector();
    const math::Vector3 fwd = camera.forward();

    math::Vector3 dir =
        right * (ndcX * aspect * tanHalfY) +
        up * (ndcY * tanHalfY) +
        fwd * (-1.0f);
    return { camera.position, dir.normalized() };
}

void Picker::worldBounds(const scene::GameObject& object, const scene::MeshComponent& mesh,
                         math::Vector3& outMin, math::Vector3& outMax) {
    outMin = math::Vector3(1e9f, 1e9f, 1e9f);
    outMax = math::Vector3(-1e9f, -1e9f, -1e9f);
    if (!mesh.data()) {
        return;
    }
    const math::Matrix4 world = object.worldMatrix();
    for (const render::Vertex& vertex : mesh.data()->vertices) {
        const math::Vector3 p = world.transformPoint(vertex.position);
        outMin.x = std::min(outMin.x, p.x);
        outMin.y = std::min(outMin.y, p.y);
        outMin.z = std::min(outMin.z, p.z);
        outMax.x = std::max(outMax.x, p.x);
        outMax.y = std::max(outMax.y, p.y);
        outMax.z = std::max(outMax.z, p.z);
    }
    // Evita AABBs degenerados.
    outMin -= math::Vector3(0.001f, 0.001f, 0.001f);
    outMax += math::Vector3(0.001f, 0.001f, 0.001f);
}

scene::GameObject* Picker::pickObject(const scene::Scene& scene, const Ray& ray) {
    scene::GameObject* best = nullptr;
    f32 bestT = 1e9f;
    for (scene::GameObject* object : scene.allObjects()) {
        if (!object->visible || !object->selectable) {
            continue;
        }
        const auto mesh = object->getComponent<scene::MeshComponent>();
        if (!mesh || !mesh->data()) {
            continue;
        }
        math::Vector3 minP, maxP;
        worldBounds(*object, *mesh, minP, maxP);
        f32 t = bestT;
        if (ray.hitAABB(minP, maxP, t)) {
            if (t < bestT) {
                bestT = t;
                best = object;
            }
        }
    }
    return best;
}

} // namespace ue::editor