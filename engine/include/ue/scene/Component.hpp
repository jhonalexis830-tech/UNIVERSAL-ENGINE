#pragma once

#include "ue/core/Types.hpp"

namespace ue::scene {

class GameObject;

/// Base de todos los componentes (Composición sobre herencia).
class Component {
public:
    virtual ~Component() = default;

    GameObject* owner() const { return owner_; }

protected:
    friend class GameObject;
    void setOwner(GameObject* owner) { owner_ = owner; }

private:
    GameObject* owner_ = nullptr;
};

} // namespace ue::scene