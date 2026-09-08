#include "ue/editor/PanelMenuBar.hpp"

#include "ue/animation/AnimationClip.hpp"
#include "ue/animation/Animator.hpp"
#include "ue/core/Log.hpp"
#include "ue/core/SystemPaths.hpp"
#include "ue/editor/EditorActions.hpp"
#include "ue/math/Color.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/components/LightComponent.hpp"

#include <imgui.h>

#include <cstring>
#include <memory>

namespace ue::editor {

PanelMenuBar::PanelMenuBar(EditorState& state)
    : state_(state) {}

void PanelMenuBar::draw() {
    if (!ImGui::BeginMainMenuBar()) {
        return;
    }

    if (ImGui::BeginMenu("Archivo")) {
        drawFileMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Editar")) {
        if (ImGui::MenuItem("Herramienta: Seleccionar", "Q")) state_.tool = Tool::Seleccionar;
        if (ImGui::MenuItem("Herramienta: Mover", "W")) state_.tool = Tool::Mover;
        if (ImGui::MenuItem("Herramienta: Rotar", "R")) state_.tool = Tool::Rotar;
        if (ImGui::MenuItem("Herramienta: Escalar", "E")) state_.tool = Tool::Escalar;
        if (ImGui::MenuItem("Herramienta: Editar malla", "T")) state_.tool = Tool::EditarMalla;
        ImGui::Separator();
        if (ImGui::MenuItem("Eliminar selección", "Supr", false, state_.selected != nullptr)) {
            EditorActions::deleteObject(state_, state_.selected);
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Objeto")) {
        drawObjectMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Vista")) {
        drawViewMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Grabar")) {
        drawRecordMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Ayuda")) {
        drawHelpMenu();
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
    drawModalWindows();
}

void PanelMenuBar::drawFileMenu() {
    if (ImGui::MenuItem("Nuevo")) {
        EditorActions::newScene(state_);
        state_.toast = "Nueva escena creada.";
    }
    if (ImGui::MenuItem("Abrir...")) {
        openedLoad_ = true;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Guardar")) {
        const std::string saved = core::SystemPaths::projectsDirectory() + "\\escena_";
        if (filePath_[0] == '\0') {
            std::strncpy(filePath_, saved.c_str(), sizeof(filePath_) - 1);
            openedSaveAs_ = true;
        } else {
            EditorActions::saveScene(state_, filePath_);
            state_.toast = std::string("Escena guardada en ") + filePath_;
        }
    }
    if (ImGui::MenuItem("Guardar como...")) {
        openedSaveAs_ = true;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Salir")) {
        state_.exitRequested = true;
    }
}

void PanelMenuBar::drawObjectMenu() {
    if (ImGui::BeginMenu("Añadir")) {
        if (ImGui::MenuItem("Cubo")) EditorActions::addPrimitive(state_, "Cubo", "cubo", 1.0f, math::Color(0.35f, 0.55f, 0.85f, 1.0f));
        if (ImGui::MenuItem("Plano")) EditorActions::addPrimitive(state_, "Plano", "plano", 1.0f, math::Color(0.16f, 0.19f, 0.24f, 1.0f));
        if (ImGui::MenuItem("Esfera")) EditorActions::addPrimitive(state_, "Esfera", "esfera", 1.0f, math::Color(0.92f, 0.66f, 0.26f, 1.0f));
        if (ImGui::MenuItem("Cilindro")) EditorActions::addPrimitive(state_, "Cilindro", "cilindro", 1.0f, math::Color(0.20f, 0.55f, 0.32f, 1.0f));
        if (ImGui::MenuItem("Cono")) EditorActions::addPrimitive(state_, "Cono", "cono", 1.0f, math::Color(0.78f, 0.28f, 0.28f, 1.0f));
        if (ImGui::MenuItem("Cápsula")) EditorActions::addPrimitive(state_, "Cápsula", "capsula", 1.0f, math::Color(0.62f, 0.65f, 0.70f, 1.0f));
        if (ImGui::MenuItem("Toro")) EditorActions::addPrimitive(state_, "Toro", "toro", 1.0f, math::Color(0.55f, 0.32f, 0.72f, 1.0f));
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Luz")) {
        if (ImGui::MenuItem("Sol (direccional)")) EditorActions::addLight(state_, 0);
        if (ImGui::MenuItem("Puntual")) EditorActions::addLight(state_, 1);
        if (ImGui::MenuItem("Foco")) EditorActions::addLight(state_, 2);
        ImGui::EndMenu();
    }
}

void PanelMenuBar::drawViewMenu() {
    if (ImGui::MenuItem("Rejilla", nullptr, &state_.showGrid)) {}
    if (ImGui::MenuItem("Ejes de referencia", nullptr, &state_.showAxis)) {}
    if (ImGui::MenuItem("Sombreado", nullptr, &state_.shaded)) {}
    ImGui::Separator();
    if (ImGui::MenuItem("Buscar objeto (cámara)", "F", false,
                        state_.selected != nullptr)) {
        // La cámara se centra en la selección (se gestiona en UiSystem).
        state_.toast = "Cámara enfocada en la selección.";
    }
}

void PanelMenuBar::drawRecordMenu() {
    if (ImGui::MenuItem("Iniciar grabación", "Ctrl+R", false, !state_.recording)) {
        state_.recording = true;
        state_.recorder.beginSession(core::SystemPaths::recordingsDirectory(), state_.scene.name);
        state_.toast = "Grabando video (secuencia BMP)...";
    }
    if (ImGui::MenuItem("Detener grabación", "Ctrl+R", false, state_.recording)) {
        state_.recording = false;
        const std::string folder = state_.recorder.endSession(60.0);
        state_.toast = "Grabación finalizada en " + folder;
    }
    if (ImGui::MenuItem("Capturar imagen de pantalla")) {
        EditorActions::exportFrame(state_);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Grabación de animación (auto-key)", nullptr, &state_.animRecording)) {}
    if (ImGui::MenuItem("Reproducir animación grabada", nullptr, state_.animator.isPlaying())) {
        if (state_.lastAnimationClip) {
            state_.animator.play(
                std::static_pointer_cast<const anim::AnimationClip>(state_.lastAnimationClip), true);
            state_.toast = "Reproduciendo: " + state_.lastAnimationClip->name;
        } else {
            state_.toast = "Graba una animación antes de reproducirla.";
        }
    }
    if (ImGui::MenuItem("Detener animación", nullptr, false, state_.animator.isPlaying())) {
        state_.animator.stop();
        state_.toast = "Animación detenida.";
    }
}

void PanelMenuBar::drawHelpMenu() {
    if (ImGui::MenuItem("Acerca de...")) {
        ImGui::OpenPopup("AcercaDe");
    }
}

void PanelMenuBar::drawModalWindows() {
    // Diálogo de guardar
    if (openedSaveAs_) {
        ImGui::OpenPopup("Guardar escena");
        openedSaveAs_ = false;
    }
    if (ImGui::BeginPopupModal("Guardar escena", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Ruta del proyecto (.ueproj):");
        ImGui::InputText("##rutaGuardar", filePath_, sizeof(filePath_),
                         ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::Button("Guardar")) {
            std::string path = filePath_;
            if (path.rfind(".ueproj") == std::string::npos) {
                path += ".ueproj";
            }
            EditorActions::saveScene(state_, path);
            state_.toast = "Escena guardada en " + path;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancelar")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Diálogo de abrir
    if (openedLoad_) {
        ImGui::OpenPopup("Abrir escena");
        openedLoad_ = false;
    }
    if (ImGui::BeginPopupModal("Abrir escena", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Archivo a abrir (.ueproj):");
        ImGui::InputText("##rutaAbrir", filePath_, sizeof(filePath_),
                         ImGuiInputTextFlags_EnterReturnsTrue);
if (ImGui::Button("Abrir")) {
            if (EditorActions::loadScene(state_, filePath_)) {
                state_.toast = "Escena cargada.";
            } else {
                state_.toast = "No se pudo abrir " + std::string(filePath_);
                core::Log::instance().warn(state_.toast);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancelar##abrir")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Acerca de
    if (ImGui::BeginPopupModal("Acerca de", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("UNIVERSAL ENGINE");
        ImGui::TextUnformatted("Motor gráfico 3D multi-herramienta.");
        ImGui::Separator();
        ImGui::TextUnformatted("Desarrollado por TEAM S.H.O.T. WARE.");
        ImGui::TextUnformatted("Fase 1 — Visor y edición básica.");
        ImGui::TextUnformatted("Motor de renderizado: OpenGL 4.3.");
        ImGui::Spacing();
        if (ImGui::Button("Cerrar")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

} // namespace ue::editor