#pragma once

#include "ue/editor/IPanel.hpp"
#include "ue/editor/EditorState.hpp"

namespace ue::editor {

/// Menú superior del editor (Archivo, Editar, Objeto, Vista, Grabar, Ayuda).
class PanelMenuBar final : public IPanel {
public:
    explicit PanelMenuBar(EditorState& state);

    const char* title() const override { return "Menú"; }
    void draw() override;

private:
    void drawFileMenu();
    void drawObjectMenu();
    void drawViewMenu();
    void drawRecordMenu();
    void drawHelpMenu();
    void drawModalWindows();

    EditorState& state_;
    char filePath_[1024] = {};
    bool openedLoad_ = false;
    bool openedSaveAs_ = false;
    std::string lastError_;
};

} // namespace ue::editor