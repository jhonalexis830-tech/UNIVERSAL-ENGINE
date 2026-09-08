#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Color.hpp"

#include <memory>
#include <string>

namespace ue::scene {
class GameObject;
class LightComponent;
struct Scene;
}

namespace ue::editor {

struct EditorState;

/// Acciones del editor reutilizables por todos los paneles (menú, barra de
/// herramientas, jerarquía...). Centralizan la lógica de modificación de la
/// escena para evitar duplicar código y garantizar transiciones coherentes.
namespace EditorActions {

    /// Crea un GameObject con malla + material y lo selecciona.
    std::shared_ptr<scene::GameObject> addPrimitive(EditorState& state, const std::string& name,
                                                    const std::string& primitive, f32 sizeScale,
                                                    const math::Color& color);

    /// Añade una luz (Sol/Punto/Foco) a la escena y la selecciona.
    std::shared_ptr<scene::GameObject> addLight(EditorState& state, int kind);

    /// Elimina un objeto de la escena (y sus hijos).
    void deleteObject(EditorState& state, scene::GameObject* object);

    /// Guarda la escena actual en la ruta dada.
    void saveScene(const EditorState& state, const std::string& path);

    /// Carga una escena desde la ruta dada reemplazando la actual.
    bool loadScene(EditorState& state, const std::string& path);

    /// Crea una escena limpia con rejilla y luz (usado por "Nuevo").
    void newScene(EditorState& state);

    /// Exporta el fotograma actual del viewport como BMP en Capturas.
    void exportFrame(EditorState& state);

} // namespace EditorActions

} // namespace ue::editor