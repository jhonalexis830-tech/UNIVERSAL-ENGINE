#include "ue/editor/EditorActions.hpp"
#include "ue/editor/EditorState.hpp"

#include "ue/core/Log.hpp"
#include "ue/core/SystemPaths.hpp"
#include "ue/media/BmpImage.hpp"
#include "ue/render/Camera.hpp"
#include "ue/scene/MeshFactory.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/SceneSerializer.hpp"
#include "ue/scene/components/LightComponent.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <filesystem>

namespace ue::editor {

namespace {
    std::shared_ptr<scene::GameObject> spawnPrimitive(scene::Scene& scene, const std::string& name,
                                                      render::MeshDataPtr mesh,
                                                      const math::Color& color,
                                                      const std::string& primitive) {
        auto material = std::make_shared<render::Material>();
        material->baseColor = color;
        material->shininess = 42.0f;
        auto object = scene.createObject(name);
        auto meshComponent = object->addComponent<scene::MeshComponent>(mesh, material);
        meshComponent->primitiveType = primitive;
        scene.addObject(object);
        return object;
    }
} // namespace

std::shared_ptr<scene::GameObject> EditorActions::addPrimitive(EditorState& state,
                                                               const std::string& name,
                                                               const std::string& primitive,
                                                               f32 sizeScale,
                                                               const math::Color& color) {
    std::shared_ptr<scene::GameObject> object;
    if (primitive == "cubo") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::cube(1.0f * sizeScale), color, primitive);
    } else if (primitive == "plano") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::plane(8.0f * sizeScale, 8.0f * sizeScale), color, primitive);
    } else if (primitive == "esfera") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::sphere(0.5f * sizeScale), color, primitive);
    } else if (primitive == "cilindro") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::cylinder(0.5f * sizeScale, 1.0f * sizeScale), color, primitive);
    } else if (primitive == "cono") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::cone(0.5f * sizeScale, 1.0f * sizeScale), color, primitive);
    } else if (primitive == "capsula") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::capsule(2.0f * sizeScale, 0.5f * sizeScale), color, primitive);
    } else if (primitive == "toro") {
        object = spawnPrimitive(state.scene, name, scene::MeshFactory::torus(0.7f * sizeScale, 0.22f * sizeScale), color, primitive);
    } else {
        return nullptr;
    }
    if (object) {
        state.selected = object.get();
        state.selectedVertex = -1;
    }
    return object;
}

std::shared_ptr<scene::GameObject> EditorActions::addLight(EditorState& state, int kind) {
    auto object = state.scene.createObject(kind == 0 ? "Sol" : (kind == 1 ? "Luz Puntual" : "Foco"));
    auto light = object->addComponent<scene::LightComponent>();
    light->kind = static_cast<scene::LightComponent::Kind>(kind);
    light->color = math::Color(1.0f, 0.98f, 0.94f, 1.0f);
    light->intensity = 1.0f;
    if (kind == 1) {
        object->transform.position = { 3.0f, 5.0f, 2.0f };
        light->position = object->transform.position;
    } else if (kind == 2) {
        object->transform.position = { 0.0f, 4.0f, 0.0f };
        light->position = object->transform.position;
        light->direction = { 0.0f, -1.0f, 0.0f };
    }
    state.scene.addObject(object);
    state.selected = object.get();
    return object;
}

void EditorActions::deleteObject(EditorState& state, scene::GameObject* object) {
    if (!object) {
        return;
    }
    if (state.selected == object) {
        state.selected = nullptr;
        state.selectedVertex = -1;
    }
    state.scene.removeObject(object);
}

void EditorActions::saveScene(const EditorState& state, const std::string& path) {
    scene::SceneSerializer::saveToFile(state.scene, path);
}

bool EditorActions::loadScene(EditorState& state, const std::string& path) {
    if (!scene::SceneSerializer::loadFromFile(state.scene, path)) {
        return false;
    }
    state.selected = nullptr;
    state.selectedVertex = -1;
    return true;
}

void EditorActions::newScene(EditorState& state) {
    state.scene.clear();
    state.selected = nullptr;
    state.selectedVertex = -1;
    // Luz por defecto para que la escena no quede a oscuras.
    auto light = state.scene.createObject("Sol");
    auto lightComponent = light->addComponent<scene::LightComponent>();
    lightComponent->kind = scene::LightComponent::Kind::Sol;
    state.scene.addObject(light);
}

void EditorActions::exportFrame(EditorState& state) {
    std::vector<u8> rgba;
    state.renderer.readViewportRGBA(rgba);
    if (rgba.empty()) {
        core::Log::instance().warn("No se pudo capturar el viewport (FBO vacío).");
        return;
    }
    const std::string folder = core::SystemPaths::capturesDirectory();
    std::filesystem::create_directories(folder);
    const std::string path = folder + "\\captura_" + core::SystemPaths::timestampName() + ".bmp";
    if (ue::media::BmpImage::writeBmp24(path, state.renderer.viewportWidth(),
                                        state.renderer.viewportHeight(), rgba.data())) {
        state.toast = "Captura exportada: " + path;
        core::Log::instance().info(state.toast);
    } else {
        state.toast = "Error al exportar la captura.";
        core::Log::instance().warn(state.toast);
    }
}

} // namespace ue::editor