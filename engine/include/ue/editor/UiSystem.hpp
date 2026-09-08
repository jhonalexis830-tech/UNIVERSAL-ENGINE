#pragma once

#include "ue/editor/AnimationRecorder.hpp"
#include "ue/editor/EditorState.hpp"
#include "ue/editor/IEditorUserInterface.hpp"
#include "ue/math/Vector3.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace ue::scene {
class Scene;
}

namespace ue::render {
class IRenderer;
}

namespace ue::platform {
class IInput;
}

namespace ue::physics {
class PhysicsWorld;
}

namespace ue::media {
class FrameRecorder;
}

namespace ue::anim {
class Animator;
}

namespace ue::editor {

class IPanel;
class CameraController;
class TransformGizmo;

/// Sistema de interfaz del editor: reúne el estado, el control de cámara, el
/// gizmo y los paneles flotantes, y procesa los atajos de teclado globales.
class UiSystem final : public IEditorUserInterface {
public:
    UiSystem(scene::Scene& scene, render::IRenderer& renderer, platform::IInput& input,
             physics::PhysicsWorld& physics, media::FrameRecorder& recorder,
             anim::Animator& animator);

    ~UiSystem() override;

    EditorState& state() override { return state_; }
    void update(f32 deltaSeconds) override;

    /// Enfoca la cámara en un punto del mundo.
    void focusCamera(const math::Vector3& worldPoint);

private:
    void processGlobalKeys();
    void handleAnimationRecording();
    void buildStatusLine();
    void drawStatusBar();

    bool isPressed(int glfwKey);

    EditorState state_;
    CameraController* cameraController_ = nullptr;
    TransformGizmo* gizmo_ = nullptr;
    std::vector<std::unique_ptr<IPanel>> panels_;
    std::unordered_map<int, bool> previousKeys_;

    AnimationRecorder animRecorder_;

    // Clip grabado por el auto-key (para poder reproducirlo después).
    std::shared_ptr<anim::AnimationClip> ownedClip_;
    f32 animationClock_ = 0.0f;
    bool previousAnimationRec_ = false;
};

} // namespace ue::editor