#include "ue/scene/SceneSerializer.hpp"

#include "ue/core/Log.hpp"
#include "ue/math/Color.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/components/MeshComponent.hpp"
#include "ue/scene/components/LightComponent.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <vector>

namespace ue::scene {

namespace {
    using nlohmann::json;

    json vec3ToJson(const math::Vector3& v) {
        return json::array({ v.x, v.y, v.z });
    }
    math::Vector3 vec3FromJson(const json& j, const math::Vector3& fallback = {}) {
        if (!j.is_array() || j.size() < 3) {
            return fallback;
        }
        return { j[0].get<f32>(), j[1].get<f32>(), j[2].get<f32>() };
    }
    json vec4ToJson(const math::Color& c) {
        return json::array({ c.r, c.g, c.b, c.a });
    }
    math::Color vec4FromJson(const json& j, const math::Color& fallback = math::Color::white()) {
        if (!j.is_array() || j.size() < 4) {
            return fallback;
        }
        return { j[0].get<f32>(), j[1].get<f32>(), j[2].get<f32>(), j[3].get<f32>() };
    }

    json meshDataToJson(const render::MeshData& mesh) {
        json vertices = json::array();
        for (const render::Vertex& v : mesh.vertices) {
            vertices.push_back(json::array({
                json::array({ v.position.x, v.position.y, v.position.z }),
                json::array({ v.normal.x, v.normal.y, v.normal.z }),
                json::array({ v.uv.x, v.uv.y }),
            }));
        }
        json indices = json::array();
        for (const u32 idx : mesh.indices) {
            indices.push_back(idx);
        }
        return json{ { "vertices", vertices }, { "indices", indices } };
    }

    bool meshDataFromJson(const json& j, render::MeshData& out) {
        if (!j.contains("vertices") || !j.contains("indices")) {
            return false;
        }
        out.clear();
        const json& verts = j["vertices"];
        for (const auto& entry : verts) {
            if (!entry.is_array() || entry.size() < 3) {
                return false;
            }
            const math::Vector3 p = vec3FromJson(entry[0]);
            const math::Vector3 n = vec3FromJson(entry[1], math::Vector3::up());
            math::Vector2 uv;
            if (entry[2].is_array() && entry[2].size() >= 2) {
                uv = { entry[2][0].get<f32>(), entry[2][1].get<f32>() };
            }
            out.vertices.push_back({ p, n, uv });
        }
        for (const auto& idx : j["indices"]) {
            out.indices.push_back(idx.get<u32>());
        }
        return true;
    }
}

bool SceneSerializer::save(const Scene& scene, std::ostream& output, const std::string&) {
    json obj = json::object();
    obj["formato"]     = "UNIVERSAL ENGINE";
    obj["version"]     = kFormatVersion;
    obj["escena"]      = scene.name;
    obj["luz_sol"]     = { { "direccion", vec3ToJson(scene.globalLights.sunDirection) },
                           { "color", vec4ToJson(math::Color(scene.globalLights.sunColor.x,
                                                              scene.globalLights.sunColor.y,
                                                              scene.globalLights.sunColor.z, 1.0f)) },
                           { "ambiente", vec3ToJson(scene.globalLights.ambient) } };
    obj["camera"]      = { { "posicion", vec3ToJson(scene.camera.position) },
                           { "objetivo", vec3ToJson(scene.camera.target) },
                           { "fov", scene.camera.fovYDegrees } };

    json objects = json::array();
    for (GameObject* object : scene.allObjects()) {
        const auto mesh = object->getComponent<MeshComponent>();
        const auto light = object->getComponent<LightComponent>();

        json entry;
        entry["nombre"]   = object->name;
        entry["visible"]  = object->visible;
        entry["seleccionable"] = object->selectable;
        entry["posicion"] = vec3ToJson(object->transform.position);
        entry["rotacion"] = vec3ToJson(object->transform.rotationDegrees);
        entry["escala"]   = vec3ToJson(object->transform.scale);

        if (mesh) {
            entry["primitiva"] = mesh->primitiveType;
            if (mesh->material()) {
                entry["material"] = { { "color", vec4ToJson(mesh->material()->baseColor) },
                                      { "brillo", mesh->material()->shininess } };
            }
            entry["malla"] = meshDataToJson(*mesh->data());
        }
        if (light) {
            entry["luz"] = { { "tipo", static_cast<i32>(light->kind) },
                             { "direccion", vec3ToJson(light->direction) },
                             { "posicion", vec3ToJson(light->position) },
                             { "intensidad", light->intensity },
                             { "alcance", light->range } };
        }
        objects.push_back(entry);
    }
    obj["objetos"] = objects;

    output << obj.dump(2) << "\n";
    return static_cast<bool>(output);
}

bool SceneSerializer::load(Scene& scene, std::istream& input) {
    json obj;
    try {
        input >> obj;
    } catch (const std::exception&) {
        core::Log::instance().error("El archivo de escena no es JSON válido.");
        return false;
    }

    const std::string formato = obj.value("formato", "");
    if (formato != "UNIVERSAL ENGINE") {
        core::Log::instance().error("El archivo no parece una escena de UNIVERSAL ENGINE.");
        return false;
    }

    scene.name = obj.value("escena", "Escena importada");
    if (obj.contains("camera")) {
        scene.camera.position = vec3FromJson(obj["camera"].value("posicion", json::array()));
        scene.camera.target   = vec3FromJson(obj["camera"].value("objetivo", json::array()));
        scene.camera.fovYDegrees = obj["camera"].value("fov", 55.0f);
    }
    if (obj.contains("luz_sol")) {
        scene.globalLights.sunDirection = vec3FromJson(obj["luz_sol"].value("direccion", json::array()),
                                                       math::Vector3(0.5f, -1.0f, 0.3f).normalized());
        scene.globalLights.ambient = vec3FromJson(obj["luz_sol"].value("ambiente", json::array()),
                                                  math::Vector3(0.16f, 0.18f, 0.24f));
    }

    scene.clear();
    const json& objects = obj["objetos"];
    for (const auto& entry : objects) {
        auto object = std::make_shared<GameObject>(entry.value("nombre", "Objeto"));
        object->visible = entry.value("visible", true);
        object->selectable = entry.value("seleccionable", true);
        object->transform.position = vec3FromJson(entry.value("posicion", json::array()));
        object->transform.rotationDegrees = vec3FromJson(entry.value("rotacion", json::array()));
        object->transform.scale = vec3FromJson(entry.value("escala", json::array()), math::Vector3::one());

        if (entry.contains("malla")) {
            auto meshData = std::make_shared<render::MeshData>();
            if (meshDataFromJson(entry["malla"], *meshData) && !meshData->isEmpty()) {
                auto material = std::make_shared<render::Material>();
                if (entry.contains("material")) {
                    material->baseColor = vec4FromJson(entry["material"].value("color", json::array()));
                    material->shininess = entry["material"].value("brillo", 48.0f);
                }
                auto mesh = object->addComponent<MeshComponent>(meshData, material);
                mesh->primitiveType = entry.value("primitiva", "");
            }
        }
        if (entry.contains("luz")) {
            auto light = object->addComponent<LightComponent>();
            light->kind = static_cast<LightComponent::Kind>(entry["luz"].value("tipo", 0));
            light->direction = vec3FromJson(entry["luz"].value("direccion", json::array()));
            light->position = vec3FromJson(entry["luz"].value("posicion", json::array()));
            light->intensity = entry["luz"].value("intensidad", 1.0f);
            light->range = entry["luz"].value("alcance", 20.0f);
        }
        scene.addObject(object);
    }
    return true;
}

bool SceneSerializer::saveToFile(const Scene& scene, const std::string& path) {
    std::ofstream file(path, std::ios::trunc);
    if (!file) {
        core::Log::instance().errorFmt("No se pudo escribir el archivo '{}'", path);
        return false;
    }
    return save(scene, file, path);
}

bool SceneSerializer::loadFromFile(Scene& scene, const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        core::Log::instance().errorFmt("No se pudo abrir el archivo '{}'", path);
        return false;
    }
    return load(scene, file);
}

} // namespace ue::scene