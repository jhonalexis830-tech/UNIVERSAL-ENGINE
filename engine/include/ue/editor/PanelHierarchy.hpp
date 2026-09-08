#pragma once

#include "ue/editor/IPanel.hpp"
#include "ue/editor/EditorState.hpp"

namespace ue::scene {
class GameObject;
}

namespace ue::editor {

/// Panel de jerarquía de la escena: lista de objetos, selección y visibilidad.
class PanelHierarchy final : public IPanel {
public:
    explicit PanelHierarchy(EditorState& state);

    const char* title() const override { return "Jerarquía"; }
    void draw() override;

private:
    void drawObjectNode(scene::GameObject& object);
    void drawAddPopup();

    EditorState& state_;
    int nextId_ = 0;
};

} // namespace ue::editor