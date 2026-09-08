#include "ue/physics/Physics.hpp"

#include "ue/core/Log.hpp"
#include "ue/scene/Scene.hpp"

#include <algorithm>

namespace ue::physics {

void PhysicsWorld::registerBody(const std::shared_ptr<scene::GameObject>& object, f32 radius) {
    if (!object) {
        return;
    }
    auto body = std::make_unique<Body>();
    body->object = object;
    body->rb.radius = radius;
    bodies_.push_back(std::move(body));
}

void PhysicsWorld::unregister(scene::GameObject* object) {
    bodies_.erase(std::remove_if(bodies_.begin(), bodies_.end(),
                                 [object](const std::unique_ptr<Body>& b) {
                                     return b->object.get() == object;
                                 }),
                  bodies_.end());
}

void PhysicsWorld::update(f32 deltaSeconds, scene::Scene& scene) {
    (void)scene;
    for (auto& bodyPtr : bodies_) {
        Body& body = *bodyPtr;
        if (!body.rb.dynamic || !body.object) {
            continue;
        }
        RigidBody& rb = body.rb;

        rb.velocity += gravity_ * deltaSeconds;

        math::Vector3& pos = body.object->transform.position;
        pos += rb.velocity * deltaSeconds;

        // Suelo en y = 0 (el piso de la escena base).
        const f32 floorY = rb.radius - 0.0f;
        if (pos.y < floorY) {
            pos.y = floorY;
            if (rb.velocity.y < 0.0f) {
                rb.velocity.y = -rb.velocity.y * rb.restitution;
            }
            // Fricción horizontal al aterrizar.
            const f32 damp = std::max(0.0f, 1.0f - rb.friction * deltaSeconds * 8.0f);
            rb.velocity.x *= damp;
            rb.velocity.z *= damp;
        }
    }
}

} // namespace ue::physics