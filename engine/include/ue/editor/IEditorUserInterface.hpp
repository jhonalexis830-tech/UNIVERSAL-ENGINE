#pragma once

#include "ue/core/Types.hpp"

namespace ue::editor {

struct EditorState;

/// Contrato de la interfaz de usuario del editor. El resto del motor solo
/// depende de esta interfaz (never depend on the concrete panel set).
class IEditorUserInterface {
public:
    virtual ~IEditorUserInterface() = default;

    virtual EditorState& state() = 0;

    /// Dibuja todos los paneles del fotograma actual.
    virtual void update(f32 deltaSeconds) = 0;
};

} // namespace ue::editor