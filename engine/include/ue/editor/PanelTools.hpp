#pragma once

#include "ue/editor/IPanel.hpp"
#include "ue/editor/EditorState.hpp"

namespace ue::editor {

/// Barra de herramientas vertical izquierda: selección de herramienta y
/// alternadores de vista.
class PanelTools final : public IPanel {
public:
    explicit PanelTools(EditorState& state);

    const char* title() const override { return "Herramientas"; }
    void draw() override;

private:
    void toolButton(const char* glyph, Tool tool, const char* tooltip);

    EditorState& state_;
};

} // namespace ue::editor