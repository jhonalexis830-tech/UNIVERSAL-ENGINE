#pragma once

#include "ue/core/Types.hpp"
#include "ue/core/Uuid.hpp"
#include "ue/render/Camera.hpp"
#include "ue/scene/GameObject.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ue::scene {

/// Escena del editor: colección de GameObjects + luces + cámara por defecto.
class Scene {
public:
    explicit Scene(std::string name = "Escena sin título");

    std::string name;
    std::vector<std::shared_ptr<GameObject>> rootObjects;
    render::Lights globalLights = render::Lights::defaultLights();
    render::Camera camera; // cámara del viewport (grabable y capturable)

    std::shared_ptr<GameObject> createObject(const std::string& objectName);
    void addObject(const std::shared_ptr<GameObject>& object);

    /// Elimina un objeto (lo busque donde lo busque) y sus descendientes.
    bool removeObject(const GameObject* object);

    /// Lista plana de todos los objetos (preorden DFS).
    std::vector<GameObject*> allObjects() const;

    GameObject* find(const core::Uuid& uuid) const;
    GameObject* findByName(const std::string& objectName) const;

    void clear();

    /// Escena de demostración con estructuras pre-integradas.
    static std::shared_ptr<Scene> createTemplateScene();

private:
    void collect(GameObject& object, std::vector<GameObject*>& out) const;
    void eraseDeep(std::vector<std::shared_ptr<GameObject>>& list, const GameObject* target);
};

} // namespace ue::scene