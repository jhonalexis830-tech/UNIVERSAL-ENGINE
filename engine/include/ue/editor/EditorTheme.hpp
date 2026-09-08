#pragma once

#include "ue/core/Types.hpp"

namespace ue::editor {

/// Paleta del editor: NEGRO PROFUNDO y AZUL METÁLICO (identidad del motor).
/// Define los colores y evita repetirlos por todo el código (DRY).
class EditorTheme {
public:
    struct Colors {
        // Fondos
        float deepBlack[4]    = { 0.043f, 0.051f, 0.075f, 1.0f };   // fondo global
        float panelBackground[4] = { 0.059f, 0.072f, 0.104f, 1.0f };
        float panelDark[4]    = { 0.047f, 0.055f, 0.082f, 1.0f };
        float frameBackground[4] = { 0.082f, 0.098f, 0.140f, 1.0f };
        // Azul metálico
        float metallicBlue[4] = { 0.22f, 0.42f, 0.72f, 1.0f };
        float steelBlue[4]    = { 0.31f, 0.52f, 0.85f, 1.0f };
        float brightBlue[4]   = { 0.46f, 0.66f, 0.98f, 1.0f };
        // Texto y acentos
        float text[4]         = { 0.86f, 0.88f, 0.92f, 1.0f };
        float textMuted[4]    = { 0.55f, 0.60f, 0.68f, 1.0f };
        float accent[4]       = { 0.40f, 0.60f, 0.95f, 1.0f };
        float selection[4]    = { 0.35f, 0.56f, 0.92f, 0.32f };
    };

    static const Colors& colors();

    /// Aplica la estética completa a ImGui.
    static void applyStyle();

    /// Carga la fuente del sistema (Segoe UI en Windows) o la integrada.
    static void loadDefaultFont();
};

} // namespace ue::editor