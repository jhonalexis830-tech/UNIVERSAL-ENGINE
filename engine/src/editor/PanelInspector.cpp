#include "ue/editor/PanelInspector.hpp"

#include "ue/render/Material.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/Transform.hpp"
#include "ue/scene/components/LightComponent.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <imgui.h>

#include <cmath>
#include <memory>

namespace ue::editor {

PanelInspector::PanelInspector(EditorState& state)
    : state_(state) {}

void PanelInspector::drawTransform(scene::GameObject& object) {
    ImGui::SeparatorText("Transformación");

    ImGui::Text("Posición");
    ImGui::SameLine();
    ImGui::PushID("pos");
    ImGui::DragFloat3("##pos", &object.transform.position.x, 0.05f);
    ImGui::PopID();

    ImGui::Text("Rotación");
    ImGui::SameLine();
    ImGui::PushID("rot");
    ImGui::DragFloat3("##rot", &object.transform.rotationDegrees.x, 0.5f, -360.0f, 360.0f);
    ImGui::PopID();

    ImGui::Text("Escala");
    ImGui::SameLine();
    ImGui::PushID("esc");
    ImGui::DragFloat3("##esc", &object.transform.scale.x, 0.01f, 0.01f, 100.0f);
    ImGui::PopID();
}

void PanelInspector::drawMesh(scene::GameObject& object) {
    const auto mesh = object.getComponent<scene::MeshComponent>();
    if (!mesh || !mesh->data()) {
        return;
    }
    ImGui::SeparatorText("Malla");
    ImGui::Text("Primitiva: %s", mesh->primitiveType.empty() ? "personalizada" : mesh->primitiveType.c_str());
    ImGui::Text("Vértices: %zu", mesh->data()->vertices.size());
    ImGui::Text("Triángulos: %zu", mesh->data()->indices.size() / 3);

    if (!mesh->material()) {
        ImGui::TextWrapped("Sin material (usa un color por defecto en el render).");
        return;
    }

    ImGui::Text("Brillo");
    ImGui::SameLine();
    ImGui::PushID("brillo");
    ImGui::SliderFloat("##brillo", &mesh->material()->shininess, 1.0f, 200.0f, "%.0f");
    ImGui::PopID();

    ImGui::ColorEdit4("Color base", &mesh->material()->baseColor.r,
                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    if (mesh->material()->hasTexture()) {
        ImGui::Text("Textura: activa");
    }
}

void PanelInspector::drawLight(scene::GameObject& object) {
    const auto light = object.getComponent<scene::LightComponent>();
    if (!light) {
        return;
    }
    ImGui::SeparatorText("Luz");
    const char* kinds[] = { "Sol", "Puntual", "Foco" };
    int current = static_cast<int>(light->kind);
    ImGui::Text("Tipo");
    ImGui::SameLine();
    ImGui::PushID("luzTipo");
    ImGui::Combo("##luzTipo", &current, kinds, 3);
    ImGui::PopID();
    light->kind = static_cast<scene::LightComponent::Kind>(current);

    ImGui::ColorEdit4("Color de luz", &light->color.r,
                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::Text("Intensidad");
    ImGui::SameLine();
    ImGui::PushID("intensidad");
    ImGui::SliderFloat("##intensidad", &light->intensity, 0.0f, 5.0f, "%.2f");
    ImGui::PopID();
}

void PanelInspector::drawPhysicsButtons(scene::GameObject& object) {
    ImGui::SeparatorText("Física");
    // registerBody exige un shared_ptr del objeto raíz; por simplicidad solo se
    // ofrece para objetos del nivel raíz de la escena.
    std::shared_ptr<scene::GameObject> root;
    for (const auto& candidate : state_.scene.rootObjects) {
        if (candidate.get() == &object) {
            root = candidate;
            break;
        }
    }
    if (!root) {
        ImGui::TextWrapped("La física rígida solo se aplica a objetos de nivel raíz.");
        return;
    }
    if (ImGui::Button("Hacer cuerpo rígido")) {
        f32 radius = 0.5f;
        const auto mesh = object.getComponent<scene::MeshComponent>();
        if (mesh && mesh->data() && !mesh->data()->vertices.empty()) {
            f32 maxR = 0.0f;
            for (const auto& vertex : mesh->data()->vertices) {
                maxR = std::max(maxR, vertex.position.length());
            }
            radius = maxR * std::max(object.transform.scale.x,
                                     std::max(object.transform.scale.y, object.transform.scale.z));
        }
        state_.physics.registerBody(root, radius);
        state_.toast = "Cuerpo rígido añadido a " + object.name;
    }
}

void PanelInspector::draw() {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 308, ImGui::GetFrameHeight()),
                            ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 420), ImGuiCond_FirstUseEver);

    ImGui::Begin("Inspector");

    if (!state_.selected) {
        ImGui::TextWrapped("Selecciona un objeto en la jerarquía o en el viewport para editar sus "
                           "propiedades.");
        ImGui::End();
        return;
    }

    scene::GameObject& object = *state_.selected;

    ImGui::Text("Objeto: %s", object.name.c_str());
    ImGui::Text("ID: %s", object.id.toString().c_str());

    bool visible = object.visible;
    if (ImGui::Checkbox("Visible", &visible)) {
        object.visible = visible;
    }

    drawTransform(object);
    drawMesh(object);
    drawLight(object);
    drawPhysicsButtons(object);

    if (state_.tool == Tool::EditarMalla) {
        ImGui::SeparatorText("Edición de malla");
        if (state_.selectedVertex >= 0) {
            ImGui::Text("Vértice seleccionado: n.%d", state_.selectedVertex);
            ImGui::TextWrapped("Arrastra el mango naranja con el ratón en el viewport.");
        } else {
            ImGui::TextWrapped("Haz clic en un vértice del viewport para editarlo.");
        }
    }

    ImGui::End();
}

} // namespace ue::editor