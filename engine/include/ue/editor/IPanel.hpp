#pragma once

namespace ue::editor {

/// Contrato de un panel de la interfaz del editor (ventana flotante de ImGui).
/// Cada panel se dibuja de forma totalmente independiente (SRP + OCP).
struct IPanel {
    virtual ~IPanel() = default;

    /// Título tipográfico del panel (en español).
    virtual const char* title() const = 0;

    /// Dibuja el contenido del panel para el frame actual.
    virtual void draw() = 0;
};

} // namespace ue::editor