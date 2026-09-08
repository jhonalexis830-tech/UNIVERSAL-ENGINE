#include "ue/editor/UiSystem.hpp"

#include "ue/animation/AnimationClip.hpp"
#include "ue/core/Log.hpp"
#include "ue/core/SystemPaths.hpp"
#include "ue/editor/CameraController.hpp"
#include "ue/editor/EditorActions.hpp"
#include "ue/editor/EditorTheme.hpp"
#include "ue/editor/PanelHierarchy.hpp"
#include "ue/editor/PanelInspector.hpp"
#include "ue/editor/PanelMenuBar.hpp"
#include "ue/editor/PanelTools.hpp"
#include "ue/editor/PanelViewport.hpp"
#include "ue/editor/TransformGizmo.hpp"
#include "ue/math/Vector3.hpp"
#include "ue/media/FrameRecorder.hpp"
#include "ue/physics/Physics.hpp"
#include "ue/platform/IInput.hpp"
#include "ue/render/IRenderer.hpp"
#include "ue/scene/Scene.hpp"

#include <imgui.h>

#include <GLFW/glfw3.h>

#include <algorithm>

namespace ue::editor {

UiSystem::UiSystem(scene::Scene& scene, render::IRenderer& renderer, platform::IInput& input,
                   physics::PhysicsWorld& physics, media::FrameRecorder& recorder,
                   anim::Animator& animator)
    : state_(scene, renderer, input, physics, recorder, animator) {
    // Herramientas del editor compartidas con los paneles.
    cameraController_ = new CameraController(scene.camera);
    gizmo_ = new TransformGizmo();

    panels_.push_back(std::make_unique<PanelMenuBar>(state_));
    panels_.push_back(std::make_unique<PanelViewport>(state_, *cameraController_, *gizmo_));
    panels_.push_back(std::make_unique<PanelHierarchy>(state_));
    panels_.push_back(std::make_unique<PanelTools>(state_));
    panels_.push_back(std::make_unique<PanelInspector>(state_));

    state_.status = "Listo";
}

UiSystem::~UiSystem() {
    delete gizmo_;
    delete cameraController_;
}

void UiSystem::focusCamera(const math::Vector3& worldPoint) {
    cameraController_->focusOn(worldPoint);
}

bool UiSystem::isPressed(int glfwKey) {
    const bool now = state_.input.isKeyDown(glfwKey);
    const bool was = previousKeys_[glfwKey];
    previousKeys_[glfwKey] = now;
    return now && !was;
}

void UiSystem::processGlobalKeys() {
    const ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) {
        return;
    }

    const bool flying = cameraController_ && cameraController_->isFlying();

    if (!flying) {
        // Herramientas: Q seleccionar, W mover, R rotar, E escalar, T editar malla.
        if (isPressed(GLFW_KEY_Q)) state_.tool = Tool::Seleccionar;
        if (isPressed(GLFW_KEY_W)) state_.tool = Tool::Mover;
        if (isPressed(GLFW_KEY_R)) state_.tool = Tool::Rotar;
        if (isPressed(GLFW_KEY_E)) state_.tool = Tool::Escalar;
        if (isPressed(GLFW_KEY_T)) state_.tool = Tool::EditarMalla;
    }

    if (isPressed(GLFW_KEY_DELETE)) {
        EditorActions::deleteObject(state_, state_.selected);
        state_.toast = "Objeto eliminado.";
    }

    if (isPressed(GLFW_KEY_F)) {
        const bool shift = state_.input.isKeyDown(GLFW_KEY_LEFT_SHIFT);
        if (shift) {
            const bool wasFlying = cameraController_ && cameraController_->isFlying();
            if (cameraController_) {
                cameraController_->setFlying(!wasFlying);
                state_.toast = wasFlying ? "Modo vuelo desactivado."
                                         : "Modo vuelo activado (WASD + E/Q + ratón).";
            }
        } else if (state_.selected && cameraController_) {
            focusCamera(state_.selected->worldPosition());
            state_.toast = "Cámara enfocada: " + state_.selected->name;
        }
    }

    const bool ctrl = state_.input.isKeyDown(GLFW_KEY_LEFT_CONTROL);
    if (ctrl) {
        if (isPressed(GLFW_KEY_G)) state_.showGrid = !state_.showGrid;
        if (isPressed(GLFW_KEY_A)) state_.showAxis = !state_.showAxis;
        if (isPressed(GLFW_KEY_S)) state_.shaded = !state_.shaded;
        if (isPressed(GLFW_KEY_R)) {
            if (!state_.recording) {
                state_.recording = true;
                state_.recorder.beginSession(core::SystemPaths::recordingsDirectory(),
                                             state_.scene.name);
                state_.toast = "Grabación iniciada.";
            } else {
                state_.recording = false;
                const std::string folder = state_.recorder.endSession(60.0);
                state_.toast = "Grabación guardada en " + folder;
            }
        }
    }
}

void UiSystem::handleAnimationRecording() {
    if (state_.animRecording && !previousAnimationRec_) {
        // Inicio del auto-key.
        if (!state_.selected) {
            state_.animRecording = false;
            state_.toast = "Selecciona un objeto para grabar su animación.";
            previousAnimationRec_ = false;
            return;
        }
        ownedClip_ = std::make_shared<anim::AnimationClip>();
        ownedClip_->name = "Animación_" + core::SystemPaths::timestampName();
        state_.lastAnimationClip = ownedClip_;
        animRecorder_.start(*ownedClip_, *state_.selected);
        animationClock_ = 0.0f;
        state_.toast = "Auto-key activo sobre: " + state_.selected->name;
    } else if (!state_.animRecording && previousAnimationRec_) {
        if (ownedClip_ && !ownedClip_->tracks.empty()) {
            ownedClip_->finalize(animationClock_);
        }
        animRecorder_.stop();
        state_.toast = "Animación grabada. Usa Grabar > Reproducir para verla.";
    }
    previousAnimationRec_ = state_.animRecording;

    if (state_.animRecording) {
        animRecorder_.recordFrame(state_.scene, animationClock_);
        animationClock_ += state_.frameDelta;
    }
}

void UiSystem::buildStatusLine() {
    std::string line = "UNIVERSAL ENGINE  |  ";
    line += toolName(state_.tool);
    line += "  |  ";
    line += state_.shaded ? "Sombreado" : "Alámbrico";
    line += "  |  ";
    line += state_.showGrid ? "Rejilla" : "Sin rejilla";
    line += "  |  ";
    const std::size_t objects = state_.scene.allObjects().size();
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%zu objetos", objects);
    line += buffer;
    if (state_.recording) {
        line += "  |  REC";
    }
    state_.status = line;
}

void UiSystem::drawStatusBar() {
    const float menuHeight = ImGui::GetFrameHeight();
    const float height = 26.0f;
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - height));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 4));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.03f, 0.05f, 0.92f));
    ImGui::Begin("##barraEstado", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::TextUnformatted(state_.status.c_str());
    ImGui::SameLine();
    const char* recording = state_.recording ? "| ● GRABANDO" : "";
    if (state_.recording) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.92f, 0.32f, 0.26f, 1.0f), "%s", recording);
    }

    // Toast (mensaje único del editor).
    if (!state_.toast.empty()) {
        state_.toastTimer += state_.frameDelta;
        if (state_.toastTimer > 4.0f) {
            state_.toast.clear();
            state_.toastTimer = 0.0f;
        } else {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.45f, 0.72f, 1.0f, 1.0f), "• %s", state_.toast.c_str());
        }
    }

    ImGui::End();
}

void UiSystem::update(f32 deltaSeconds) {
    state_.frameDelta = std::max(deltaSeconds, 1e-4f);
    state_.fps = 1.0f / state_.frameDelta;
    ++state_.frameIndex;

    processGlobalKeys();
    handleAnimationRecording();
    buildStatusLine();

    for (auto& panel : panels_) {
        if (panel->title() == std::string("Viewport")) {
            static_cast<PanelViewport*>(panel.get())->setDeltaSeconds(deltaSeconds);
            static_cast<PanelViewport*>(panel.get())->setFps(state_.fps);
        }
        panel->draw();
    }

    drawStatusBar();
}

} // namespace ue::editor