#pragma once

#include "ue/core/Time.hpp"
#include "ue/core/Types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ue::platform {
class Window;
class Input;
}

namespace ue::render {
class OpenGLRenderer;
}

namespace ue::physics {
class PhysicsWorld;
}

namespace ue::media {
class FrameRecorder;
}

namespace ue::scene {
class Scene;
}

namespace ue::anim {
class Animator;
}

namespace ue::editor {
class IEditorUserInterface;
}

/// Aplicación del editor (Fase 1). Encaja las piezas del motor en un bucle
/// principal típico: entrada -> simulación -> render -> interfaz -> presentar.
class EditorApp {
public:
    EditorApp() = default;
    ~EditorApp();

    bool initialize(int argc, char* argv[]);
    void run();
    void shutdown();

private:
    bool initImGui();
    void prepareScene(int argc, char* argv[]);
    void captureViewportIfRecording();

    std::unique_ptr<ue::platform::Window> window_;
    std::unique_ptr<ue::platform::Input> input_;
    std::unique_ptr<ue::render::OpenGLRenderer> renderer_;
    std::unique_ptr<ue::physics::PhysicsWorld> physics_;
    std::unique_ptr<ue::media::FrameRecorder> recorder_;
    std::unique_ptr<ue::anim::Animator> animator_;
    std::shared_ptr<ue::scene::Scene> scene_;
    std::unique_ptr<ue::editor::IEditorUserInterface> ui_;

    ue::core::Time time_;
    bool imguiReady_ = false;
    double lastNowSeconds_ = 0.0;
};