#include "ue/scene/Scene.hpp"

#include "ue/core/Log.hpp"
#include "ue/math/Color.hpp"
#include "ue/scene/MeshFactory.hpp"
#include "ue/scene/components/MeshComponent.hpp"
#include "ue/scene/components/LightComponent.hpp"

#include <algorithm>

namespace ue::scene {

using render::Material;
using render::MeshData;
using render::MeshDataPtr;

Scene::Scene(std::string nameValue)
    : name(std::move(nameValue)) {}

std::shared_ptr<GameObject> Scene::createObject(const std::string& objectName) {
    auto object = std::make_shared<GameObject>(objectName);
    rootObjects.push_back(object);
    return object;
}

void Scene::addObject(const std::shared_ptr<GameObject>& object) {
    if (!object) {
        return;
    }
    if (std::find(rootObjects.begin(), rootObjects.end(), object) == rootObjects.end()) {
        rootObjects.push_back(object);
    }
}

void Scene::eraseDeep(std::vector<std::shared_ptr<GameObject>>& list, const GameObject* target) {
    list.erase(std::remove_if(list.begin(), list.end(),
                              [target](const std::shared_ptr<GameObject>& o) {
                                  return o.get() == target;
                              }),
               list.end());
    for (auto& child : list) {
        child->children.erase(std::remove_if(child->children.begin(), child->children.end(),
                                            [target](const std::shared_ptr<GameObject>& c) {
                                                return c.get() == target ||
                                                       c->isDescendantOf(target);
                                            }),
                              child->children.end());
        eraseDeep(child->children, target);
    }
}

bool Scene::removeObject(const GameObject* object) {
    bool removed = false;
    rootObjects.erase(std::remove_if(rootObjects.begin(), rootObjects.end(),
                                     [object, &removed](const std::shared_ptr<GameObject>& o) {
                                         if (o.get() == object || o->isDescendantOf(object)) {
                                             removed = true;
                                             return true;
                                         }
                                         return false;
                                     }),
                      rootObjects.end());
    eraseDeep(rootObjects, object);
    if (removed) {
        return true;
    }
    for (auto& child : rootObjects) {
        const size_t before = child->children.size();
        child->children.erase(std::remove_if(child->children.begin(), child->children.end(),
                                             [object](const std::shared_ptr<GameObject>& c) {
                                                 return c.get() == object ||
                                                        c->isDescendantOf(object);
                                             }),
                              child->children.end());
        if (child->children.size() != before) {
            return true;
        }
    }
    return false;
}

void Scene::collect(GameObject& object, std::vector<GameObject*>& out) const {
    out.push_back(&object);
    for (const auto& child : object.children) {
        collect(*child, out);
    }
}

std::vector<GameObject*> Scene::allObjects() const {
    std::vector<GameObject*> out;
    for (const auto& object : rootObjects) {
        collect(*object, out);
    }
    return out;
}

GameObject* Scene::find(const core::Uuid& uuid) const {
    for (GameObject* object : allObjects()) {
        if (object->id == uuid) {
            return object;
        }
    }
    return nullptr;
}

GameObject* Scene::findByName(const std::string& objectName) const {
    for (GameObject* object : allObjects()) {
        if (object->name == objectName) {
            return object;
        }
    }
    return nullptr;
}

void Scene::clear() {
    rootObjects.clear();
}

std::shared_ptr<Scene> Scene::createTemplateScene() {
    auto scene = std::make_shared<Scene>("Escena de inicio");

    // --- Suelo (rejilla + plano sutil) -----------------------------
    {
        auto grid = scene->createObject("Rejilla de trabajo");
        auto gridMesh = std::make_shared<render::MeshData>();
        *gridMesh = *MeshFactory::gridLines(20.0f, 20);
        auto material = std::make_shared<Material>();
        material->baseColor = math::Color(0.14f, 0.20f, 0.32f, 0.6f);
        material->shininess = 0.0f;
        auto mesh = grid->addComponent<MeshComponent>(gridMesh, material);
        mesh->primitiveType = "grid";
        mesh->setData(gridMesh);
        grid->selectable = false;
    }
    {
        auto floor = scene->createObject("Piso");
        auto floorMesh = MeshFactory::plane(20.0f, 20.0f, 1, 1);
        auto material = std::make_shared<Material>();
        material->baseColor = math::Color(0.05f, 0.06f, 0.09f, 1.0f);
        material->shininess = 2.0f;
        material->albedo = render::Texture2D::createCheckerboard();
        auto mesh = floor->addComponent<MeshComponent>(floorMesh, material);
        mesh->primitiveType = "plano";
        floor->transform.setPosition(0.0f, -0.01f, 0.0f);
    }

    // --- Caja central (editable) ------------------------------------
    {
        auto cube = scene->createObject("Caja de trabajo");
        auto material = std::make_shared<Material>();
        material->baseColor = math::Color::metallicBlue();
        material->shininess = 64.0f;
        auto mesh = cube->addComponent<MeshComponent>(MeshFactory::cube(1.0f), material);
        mesh->primitiveType = "cubo";
        cube->transform.setPosition(0.0f, 0.5f, 0.0f);
        cube->transform.scale = { 1.4f, 1.4f, 1.4f };
    }

    // --- Pelota (física: gravedad y rebote) --------------------------
    {
        auto ball = scene->createObject("Pelota de simulación");
        auto material = std::make_shared<Material>();
        material->baseColor = math::Color(0.88f, 0.35f, 0.29f, 1.0f);
        material->shininess = 96.0f;
        auto mesh = ball->addComponent<MeshComponent>(MeshFactory::sphere(0.35f), material);
        mesh->primitiveType = "esfera";
        ball->transform.setPosition(-1.8f, 2.5f, 1.2f);
    }

    // --- Cubo giratorio (animación programable) ----------------------
    {
        auto spinner = scene->createObject("Turbina de ejemplo");
        auto material = std::make_shared<Material>();
        material->baseColor = math::Color(0.30f, 0.56f, 0.90f, 1.0f);
        material->shininess = 80.0f;
        auto mesh = spinner->addComponent<MeshComponent>(MeshFactory::cylinder(0.35f, 0.9f), material);
        mesh->primitiveType = "cilindro";
        spinner->transform.setPosition(2.0f, 1.2f, -1.0f);
        spinner->transform.rotationDegrees = { 30.0f, 45.0f, 0.0f };
    }

    // --- Cápsula (figura base para personajes) -----------------------
    {
        auto figure = scene->createObject("Figura de personaje");
        auto material = std::make_shared<Material>();
        material->baseColor = math::Color(0.55f, 0.60f, 0.72f, 1.0f);
        material->shininess = 24.0f;
        auto mesh = figure->addComponent<MeshComponent>(MeshFactory::capsule(2.0f, 0.5f), material);
        mesh->primitiveType = "cápsula";
        figure->transform.setPosition(-3.2f, 1.0f, 0.8f);
    }

    // --- Luz solar -----------------------------------------------------
    {
        auto sun = scene->createObject("Sol (luz direccional)");
        auto light = sun->addComponent<LightComponent>();
        light->kind = LightComponent::Kind::Sol;
        light->direction = math::Vector3(0.5f, -1.0f, 0.3f).normalized();
        light->color = math::Color(1.0f, 0.98f, 0.92f, 1.0f);
        light->intensity = 1.0f;
    }

    // --- Cámara por defecto -------------------------------------------
    scene->camera.position = { 7.0f, 5.0f, 9.0f };
    scene->camera.target   = { 0.0f, 0.9f, 0.0f };
    scene->camera.fovYDegrees = 55.0f;

    return scene;
}

} // namespace ue::scene