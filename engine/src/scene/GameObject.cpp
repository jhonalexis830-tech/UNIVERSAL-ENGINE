#include "ue/scene/GameObject.hpp"

#include <algorithm>

namespace ue::scene {

GameObject::GameObject(std::string nameValue)
    : name(std::move(nameValue)) {}

void GameObject::addChild(const std::shared_ptr<GameObject>& child) {
    if (!child) {
        return;
    }
    if (child->parent) {
        child->parent->removeChild(child.get());
    }
    child->parent = this;
    children.push_back(child);
}

void GameObject::removeChild(const GameObject* child) {
    children.erase(std::remove_if(children.begin(), children.end(),
                                  [child](const std::shared_ptr<GameObject>& c) {
                                      return c.get() == child;
                                  }),
                   children.end());
}

math::Matrix4 GameObject::worldMatrix() const {
    math::Matrix4 result = transform.matrix();
    for (const GameObject* p = parent; p != nullptr; p = p->parent) {
        result = p->transform.matrix() * result;
    }
    return result;
}

math::Vector3 GameObject::worldPosition() const {
    math::Vector3 pos = transform.position;
    for (const GameObject* p = parent; p != nullptr; p = p->parent) {
        pos += p->transform.position;
    }
    return pos;
}

bool GameObject::isDescendantOf(const GameObject* candidate) const {
    for (const GameObject* p = parent; p != nullptr; p = p->parent) {
        if (p == candidate) {
            return true;
        }
    }
    return false;
}

} // namespace ue::scene