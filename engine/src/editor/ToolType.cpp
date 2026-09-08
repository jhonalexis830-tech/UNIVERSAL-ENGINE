#include "ue/editor/ToolType.hpp"

namespace ue::editor {

const char* toolName(Tool tool) {
    switch (tool) {
        case Tool::Ninguna:      return "Ninguna";
        case Tool::Seleccionar:  return "Seleccionar";
        case Tool::Mover:        return "Mover";
        case Tool::Rotar:        return "Rotar";
        case Tool::Escalar:      return "Escalar";
        case Tool::EditarMalla:  return "Editar polígonos";
    }
    return "Desconocida";
}

} // namespace ue::editor