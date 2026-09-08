#include "ue/editor/PanelTools.hpp"

#include <imgui.h>

namespace ue::editor {

PanelTools::PanelTools(EditorState& state)
    : state_(state) {}

void PanelTools::toolButton(const char* glyph, Tool tool, const char* tooltip) {
    const bool active = (state_.tool == tool);
    if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.44f, 0.72f, 1.0f));
    }
    if (ImGui::ButtonEx(glyph, ImVec2(34, 34), ImGuiButtonFlags_None)) {
        state_.tool = tool;
    }
    if (active) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
}

void PanelTools::draw() {
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(48, -1), ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));
    ImGui::Begin("Herramientas", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar();

    toolButton("Sel", Tool::Seleccionar, "Seleccionar (Q)");
    toolButton("Mov", Tool::Mover, "Mover (W)");
    toolButton("Rot", Tool::Rotar, "Rotar (R)");
    toolButton("Esc", Tool::Escalar, "Escalar (E)");
    toolButton("Mal", Tool::EditarMalla, "Editar malla (T)");

    ImGui::Separator();

    if (ImGui::ButtonEx("G", ImVec2(34, 34), ImGuiButtonFlags_None)) {
        state_.showGrid = !state_.showGrid;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", state_.showGrid ? "Rejilla: visible" : "Rejilla: oculta");
    }
    ImGui::Separator();
    if (ImGui::ButtonEx("X", ImVec2(34, 34), ImGuiButtonFlags_None)) {
        state_.showAxis = !state_.showAxis;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", state_.showAxis ? "Ejes: visible" : "Ejes: ocultos");
    }
    ImGui::Separator();
    if (ImGui::ButtonEx("S", ImVec2(34, 34), ImGuiButtonFlags_None)) {
        state_.shaded = !state_.shaded;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", state_.shaded ? "Modo: sombreado" : "Modo: alámbrico");
    }

    ImGui::End();
}

} // namespace ue::editor