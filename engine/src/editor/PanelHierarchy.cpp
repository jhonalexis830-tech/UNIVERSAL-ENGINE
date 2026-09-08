#include "ue/editor/PanelHierarchy.hpp"

#include "ue/editor/EditorActions.hpp"
#include "ue/math/Color.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/components/LightComponent.hpp"

#include <imgui.h>

namespace ue::editor {

namespace {
    // Indicador de visibilidad en la lista de la jerarquía.
    std::string colorDot(bool visible) {
        return visible ? "\xE2\x97\x8F " : "\xE2\x97\x8B ";
    }
} // namespace

PanelHierarchy::PanelHierarchy(EditorState& state)
    : state_(state) {}

void PanelHierarchy::drawObjectNode(scene::GameObject& object) {
    ImGui::PushID(++nextId_);

    std::string label = colorDot(object.visible);
    label += object.name;
    if (!object.visible) {
        label += "  (oculto)";
    }

    const bool isSelected = (state_.selected == &object);
    if (ImGui::Selectable(label.c_str(), isSelected)) {
        state_.selected = &object;
        state_.selectedVertex = -1;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        state_.toast = "Doble clic: cámara centrada en " + object.name;
    }

    for (const auto& child : object.children) {
        drawObjectNode(*child);
    }

    ImGui::PopID();
}

void PanelHierarchy::drawAddPopup() {
    if (ImGui::Button("+ Añadir")) {
        ImGui::OpenPopup("AnadirMenu");
    }
    if (ImGui::BeginPopup("AnadirMenu")) {
        if (ImGui::MenuItem("Cubo")) EditorActions::addPrimitive(state_, "Cubo", "cubo", 1.0f, math::Color(0.35f, 0.55f, 0.85f, 1.0f));
        if (ImGui::MenuItem("Plano")) EditorActions::addPrimitive(state_, "Plano", "plano", 1.0f, math::Color(0.16f, 0.19f, 0.24f, 1.0f));
        if (ImGui::MenuItem("Esfera")) EditorActions::addPrimitive(state_, "Esfera", "esfera", 1.0f, math::Color(0.92f, 0.66f, 0.26f, 1.0f));
        if (ImGui::MenuItem("Cilindro")) EditorActions::addPrimitive(state_, "Cilindro", "cilindro", 1.0f, math::Color(0.20f, 0.55f, 0.32f, 1.0f));
        if (ImGui::MenuItem("Cono")) EditorActions::addPrimitive(state_, "Cono", "cono", 1.0f, math::Color(0.78f, 0.28f, 0.28f, 1.0f));
        if (ImGui::MenuItem("Cápsula")) EditorActions::addPrimitive(state_, "Cápsula", "capsula", 1.0f, math::Color(0.62f, 0.65f, 0.70f, 1.0f));
        if (ImGui::MenuItem("Toro")) EditorActions::addPrimitive(state_, "Toro", "toro", 1.0f, math::Color(0.55f, 0.32f, 0.72f, 1.0f));
        ImGui::Separator();
        if (ImGui::MenuItem("Sol")) EditorActions::addLight(state_, 0);
        if (ImGui::MenuItem("Luz puntual")) EditorActions::addLight(state_, 1);
        if (ImGui::MenuItem("Foco")) EditorActions::addLight(state_, 2);
        ImGui::EndPopup();
    }
}

void PanelHierarchy::draw() {
    ImGui::SetNextWindowPos(ImVec2(52, ImGui::GetFrameHeight()), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(240, 360), ImGuiCond_FirstUseEver);

    ImGui::Begin("Jerarquía");
    drawAddPopup();

    if (ImGui::BeginChild("EscenaLista")) {
        nextId_ = 0;
        for (const auto& root : state_.scene.rootObjects) {
            drawObjectNode(*root);
        }
    }
    ImGui::EndChild();

    const i32 objCount = static_cast<i32>(state_.scene.allObjects().size());
    ImGui::Separator();
    ImGui::Text("Objetos: %d", objCount);

    ImGui::End();
}

} // namespace ue::editor