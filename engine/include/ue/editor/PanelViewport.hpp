#pragma once

#include "ue/editor/IPanel.hpp"
#include "ue/editor/EditorState.hpp"

namespace ue::editor {

class CameraController;
class TransformGizmo;

/// Ventana principal del viewport 3D. Muestra la textura del renderizador a
/// pantalla completa de fondo; encima se dibujan los paneles flotantes.
/// También aplica el control de cámara (estilo Blender) y el gizmo activo.
class PanelViewport final : public IPanel {
public:
    PanelViewport(EditorState& state, CameraController& camera, TransformGizmo& gizmo);

    const char* title() const override { return "Viewport"; }
    void draw() override;

    void setDeltaSeconds(f32 dt) { deltaSeconds_ = dt; }
    void setFps(f32 fps) { fps_ = fps; }

private:
    void drawOverlayText();

    EditorState& state_;
    CameraController& camera_;
    TransformGizmo& gizmo_;
    f32 deltaSeconds_ = 1.0f / 60.0f;
    f32 fps_ = 60.0f;
};

} // namespace ue::editor