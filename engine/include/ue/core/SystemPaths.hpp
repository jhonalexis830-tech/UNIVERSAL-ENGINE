#pragma once

#include "ue/core/Types.hpp"

#include <string>
#include <vector>

namespace ue::core {

/// Rutas del sistema de archivos del motor.
/// Crea la carpeta de trabajo del usuario:
///     %USERPROFILE%\Documents\UNIVERSAL ENGINE\
/// con subcarpetas Recordings (videos), Capturas y Proyectos.
class SystemPaths {
public:
    /// Crea (si falta) el árbol de carpetas de usuario del motor.
    static bool ensureUserFolders();

    static std::string userDirectory();       // .../UNIVERSAL ENGINE
    static std::string recordingsDirectory(); // .../UNIVERSAL ENGINE/Recordings
    static std::string capturesDirectory();   // .../UNIVERSAL ENGINE/Capturas
    static std::string projectsDirectory();   // .../UNIVERSAL ENGINE/Proyectos
    static std::string engineDirectory();     // carpeta donde esta la app/templates
    static std::string templatesDirectory();  // assets/templates junto al proyecto

    /// Lista los archivos con la extensión dada dentro de un directorio.
    static std::vector<std::string> listFiles(const std::string& directory,
                                              const std::string& extension);

    /// Devuelve un nombre de archivo único con sello de tiempo (sin extensión).
    static std::string timestampName();

    /// Nombre de usuario de Windows o "usuario".
    static std::string userName();

private:
    static std::string homeDirectory();
    static std::string exeDirectory(); // carpeta donde está el ejecutable (portable)
};

} // namespace ue::core