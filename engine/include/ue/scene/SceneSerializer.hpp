#pragma once

#include "ue/core/Types.hpp"

#include <iosfwd>
#include <string>

namespace ue::scene {

class Scene;

/// Guarda y carga escenas en formato JSON (".ueproj").
/// Cumple SRP: la serialización es responsabilidad exclusiva de esta clase.
class SceneSerializer {
public:
    static constexpr i32 kFormatVersion = 1;

    /// Guarda la escena completa (objetos, transformaciones, mallas, materiales,
    /// luz solar y cámara). Devuelve false ante errores.
    static bool save(const Scene& scene, std::ostream& output, const std::string& filePath);

    /// Carga una escena desde el flujo. Devuelve false ante errores.
    static bool load(Scene& scene, std::istream& input);

    /// Atajo: guarda a un archivo.
    static bool saveToFile(const Scene& scene, const std::string& path);

    /// Atajo: carga desde un archivo.
    static bool loadFromFile(Scene& scene, const std::string& path);
};

} // namespace ue::scene