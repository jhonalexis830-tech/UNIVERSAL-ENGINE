#pragma once

#include "ue/core/Types.hpp"

namespace ue::editor {

/// Herramientas del editor (equivalente a la barra de herramientas de Blender).
enum class Tool {
    Ninguna,
    Seleccionar,   // clic para elegir objetos
    Mover,         // gizmo de traslación por ejes
    Rotar,         // gizmo de rotación
    Escalar,       // gizmo de escala
    EditarMalla,   // edición de vértices del polígono con el cursor
};

const char* toolName(Tool tool);

} // namespace ue::editor