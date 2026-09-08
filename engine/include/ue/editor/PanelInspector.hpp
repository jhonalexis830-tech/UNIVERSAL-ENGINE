#pragma once

#include "ue/editor/IPanel.hpp"
#include "ue/editor/EditorState.hpp"

namespace ue::scene {
class GameObject;
class MeshComponent;
}

namespace ue::editor {

/// Panel de inspección: propiedades del objeto y de sus componentes.
class PanelInspector final : public IPanel {
public:
    explicit PanelInspector(EditorState& state);

    const char* title() const override { return "Inspector"; }
    void draw() override;

private:
    void drawTransform(scene::GameObject& object);
    void drawMesh(scene::GameObject& object);
    void drawLight(scene::GameObject& object);
    void drawPhysicsButtons(scene::GameObject& object);

    EditorState& state_;
};

} // namespace ue::editor