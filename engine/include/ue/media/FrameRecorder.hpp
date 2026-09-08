#pragma once

#include "ue/core/Types.hpp"

#include <string>
#include <vector>

namespace ue::media {

/// Grabadora de sesiones de video.
/// Fase 1: guarda una secuencia de imágenes BMP (fotogramas) en la carpeta
/// "Recordings" del usuario; cada sesión es una subcarpeta con marca de tiempo.
/// Fase 2 (roadmap): codificación a video (AVI/MP4) con ffmpeg.
class FrameRecorder {
public:
    FrameRecorder() = default;

    /// Crea la sesión "grabación" dentro de `baseFolder`.
    bool beginSession(const std::string& baseFolder, const std::string& sceneName);

    /// Añade un fotograma (RGBA8, w*h*4; la primera fila es la superior).
    void captureFrame(const std::vector<u8>& rgba, i32 width, i32 height);

    /// Termina la sesión y escribe un manifiesto. Devuelve la carpeta creada.
    std::string endSession(f64 fps);

    bool isRecording() const { return recording_; }
    std::string sessionFolder() const { return sessionFolder_; }
    u32 frameCount() const { return frameCount_; }

private:
    bool recording_ = false;
    std::string sessionFolder_;
    std::string sceneName_;
    u32 frameCount_ = 0;
};

} // namespace ue::media