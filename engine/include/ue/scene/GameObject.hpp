#pragma once

#include "ue/core/Types.hpp"
#include "ue/core/Uuid.hpp"
#include "ue/math/Matrix4.hpp"
#include "ue/scene/Component.hpp"
#include "ue/scene/Transform.hpp"

#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ue::scene {

/// Entidad del universo 3D: identidad + transform + componentes.
class GameObject {
public:
    explicit GameObject(std::string name);

    core::Uuid id;
    std::string name;
    bool visible = true;
    bool selectable = true;

    Transform transform;
    GameObject* parent = nullptr;
    std::vector<std::shared_ptr<GameObject>> children;

    // ---- Componentes -----------------------------------------
    template <typename TComponent, typename... Args>
    std::shared_ptr<TComponent> addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, TComponent>,
                      "TComponent debe heredar de ue::scene::Component");
        auto component = std::make_shared<TComponent>(std::forward<Args>(args)...);
        component->setOwner(this);
        components_[typeid(TComponent)] = component;
        return component;
    }

    template <typename TComponent>
    std::shared_ptr<TComponent> getComponent() const {
        static_assert(std::is_base_of_v<Component, TComponent>,
                      "TComponent debe heredar de ue::scene::Component");
        const auto it = components_.find(typeid(TComponent));
        if (it == components_.end()) {
            return nullptr;
        }
        return std::static_pointer_cast<TComponent>(it->second);
    }

    template <typename TComponent>
    bool hasComponent() const {
        return components_.count(typeid(TComponent)) != 0;
    }

    // ---- Jerarquía -------------------------------------------
    void addChild(const std::shared_ptr<GameObject>& child);
    void removeChild(const GameObject* child);

    /// Matriz del mundo (encadena la transformación de los ancestros).
    math::Matrix4 worldMatrix() const;

    /// Posición del mundo aproximada (solo traslación de ancestros).
    math::Vector3 worldPosition() const;

    bool isDescendantOf(const GameObject* candidate) const;

private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components_;
};

} // namespace ue::scene