#include "ue/editor/PanelViewport.hpp"

#include "ue/editor/CameraController.hpp"
#include "ue/editor/Picker.hpp"
#include "ue/editor/TransformGizmo.hpp"
#include "ue/math/Vector2.hpp"
#include "ue/platform/IInput.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstdio>

namespace ue::editor {

PanelViewport::PanelViewport(EditorState& state, CameraController& camera, TransformGizmo& gizmo)
    : state_(state), camera_(camera), gizmo_(gizmo) {}

void PanelViewport::drawOverlayText() {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float top = state_.viewportPos.y + 8.0f;

    // Encabezado del motor.
    std::string sceneLabel = "UNIVERSAL ENGINE";
    if (!state_.scene.name.empty()) {
        sceneLabel += "  //  " + state_.scene.name;
    }
    dl->AddText(ImVec2(state_.viewportPos.x + 12.0f, top), IM_COL32(205, 215, 232, 255),
                sceneLabel.c_str());

    char stats[160];
    std::snprintf(stats, sizeof(stats), "FPS %.0f | %.1f ms | %lu objetos", fps_,
                  deltaSeconds_ * 1000.0,
                  static_cast<unsigned long>(state_.scene.allObjects().size()));
    dl->AddText(ImVec2(state_.viewportPos.x + 12.0f, top + 20.0f), IM_COL32(120, 135, 160, 255),
                stats);

    if (state_.recording) {
        dl->AddText(ImVec2(state_.viewportPos.x + 12.0f, top + 40.0f), IM_COL32(236, 82, 66, 255),
                    "REC");
    }
    if (state_.animRecording) {
        dl->AddText(ImVec2(state_.viewportPos.x + 12.0f, top + 56.0f), IM_COL32(230, 190, 90, 255),
                    "AUTO-KEY");
    }

    // Ayuda de la cámara (esquina inferior izquierda).
    const float bottom = state_.viewportPos.y + state_.viewportSize.y - 10.0f;
    dl->AddText(ImVec2(state_.viewportPos.x + 12.0f, bottom), IM_COL32(105, 120, 145, 200),
                "MMB orbitar | Shift+MMB mover | Rueda zoom | 1/3/7 vistas | Shift+F vuelo");
}

void PanelViewport::draw() {
    const float menuHeight = ImGui::GetFrameHeight();
    const ImVec2 origin(0.0f, menuHeight);
    const ImVec2 windowSize(ImGui::GetIO().DisplaySize.x,
                            std::max(ImGui::GetIO().DisplaySize.y - menuHeight, 80.0f));

    ImGui::SetNextWindowPos(origin);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Viewport", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav);
    ImGui::PopStyleVar(2);

    state_.viewportPos = math::Vector2(origin.x, origin.y);
    const ImVec2 content = ImGui::GetContentRegionAvail();
    state_.viewportSize = math::Vector2(content.x, content.y);
    state_.viewportHovered = ImGui::IsWindowHovered();
    state_.viewportFocused = ImGui::IsWindowFocused();

    if (content.x > 4.0f && content.y > 4.0f) {
        ImTextureID tex = reinterpret_cast<ImTextureID>(
            static_cast<uintptr_t>(state_.renderer.viewportColorTexture()));
        // La textura del FBO guarda la fila inferior primero, así que se
        // muestran las coordenadas V invertidas para que el cielo quede arriba.
        ImGui::Image(tex, content, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    }

    // ------- Interacción: cámara, gizmo y selección ---------------------
    const math::Vector2 mouseGlobal = state_.input.mousePosition();
    const math::Vector2 mouseLocal = mouseGlobal - state_.viewportPos;

    const bool lmbDown = state_.input.isMouseButtonDown(ue::platform::MouseButton::Left);
    const bool lmbPressed = state_.viewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if (!gizmo_.isDragging()) {
        camera_.update(state_.input, deltaSeconds_, state_.viewportHovered);
    }

    const bool flying = camera_.isFlying();

    bool meshEdited = false;
    if (!flying) {
        gizmo_.update(state_.tool, state_.scene, state_.scene.camera, mouseLocal, state_.viewportSize,
                      lmbDown, lmbPressed, state_.viewportHovered, state_.selected,
                      state_.selectedVertex, meshEdited);
    }

    if (meshEdited) {
        if (auto mesh = state_.selected
                            ? state_.selected->getComponent<scene::MeshComponent>() : nullptr) {
            if (mesh->data()) {
                mesh->data()->recomputeNormals();
                mesh->markEdited();
            }
        }
    }

    if (!flying && state_.tool != Tool::EditarMalla && lmbPressed && !gizmo_.isDragging()) {
        const Ray ray = Picker::cameraRay(state_.scene.camera, mouseLocal, state_.viewportSize);
        state_.selected = Picker::pickObject(state_.scene, ray);
        state_.selectedVertex = -1;
    }

    // ------- Overlay del gizmo y textos -----------------------------------
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (!flying) {
        gizmo_.draw(*drawList, state_.scene.camera, state_.viewportPos, state_.viewportSize,
                    state_.selected, state_.tool);
    }
    drawOverlayText();

    ImGui::End();
}

} // namespace ue::editor