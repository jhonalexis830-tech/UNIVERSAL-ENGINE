#pragma once

#include "ue/core/Types.hpp"
#include "ue/editor/ToolType.hpp"
#include "ue/math/Vector2.hpp"

#include <memory>
#include <string>

namespace ue::scene {
class Scene;
class GameObject;
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
class AnimationClip;
}

namespace ue::editor {

/// Estado mutable compartido por los paneles del editor a lo largo del frame.
/// Es el punto único de verdad para la selección, la herramienta activa y los
/// modos del viewport (SRP: los paneles solo leen/escriben aquí).
struct EditorState {
    scene::Scene& scene;
    render::IRenderer& renderer;
    platform::IInput& input;
    physics::PhysicsWorld& physics;
    media::FrameRecorder& recorder;
    anim::Animator& animator;

    // ---- Interacción -------------------------------------------------
    scene::GameObject* selected = nullptr;
    i32 selectedVertex = -1;
    Tool tool = Tool::Seleccionar;

    // ---- Viewport (píxeles lógicos) ---------------------------------
    math::Vector2 viewportPos{ 0.0f, 0.0f };
    math::Vector2 viewportSize{ 640.0f, 480.0f };
    bool viewportHovered = false;
    bool viewportFocused = false;

    // ---- Opciones de vista -------------------------------------------
    bool showGrid = true;
    bool showAxis = true;   // cerca de la cámara (marca los ejes)
    bool shaded = true;     // false = vista de alambres

    // ---- Grabación ----------------------------------------------------
    bool recording = false;     // video (secuencia BMP en Recordings)
    bool animRecording = false; // auto-key de animación en curso

    // ---- Información de la barra inferior -----------------------------
    std::string status;   // barra de estado (se reconstruye cada fotograma)
    std::string toast;    // mensaje único sobre el viewport (se muestra ~4 s)
    f32 toastTimer = 0.0f;
    f32 fps = 60.0f;
    u32 frameIndex = 0;

    // ---- Animación -----------------------------------------------------
    std::shared_ptr<anim::AnimationClip> lastAnimationClip; // clip grabado por el auto-key

    // ---- Control del bucle principal ----------------------------------
    bool exitRequested = false;
    f32 frameDelta = 0.0f; // segundos del último fotograma (para el viewport)

    explicit EditorState(scene::Scene& scene, render::IRenderer& renderer,
                         platform::IInput& input, physics::PhysicsWorld& physics,
                         media::FrameRecorder& recorder, anim::Animator& animator);
};

} // namespace ue::editor