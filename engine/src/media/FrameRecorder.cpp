#include "ue/media/FrameRecorder.hpp"

#include "ue/core/Log.hpp"
#include "ue/core/SystemPaths.hpp"
#include "ue/media/BmpImage.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace ue::media {

namespace fs = std::filesystem;

bool FrameRecorder::beginSession(const std::string& baseFolder, const std::string& sceneName) {
    std::error_code ec;
    fs::create_directories(baseFolder, ec);
    if (ec) {
        core::Log::instance().errorFmt("No se pudo crear la carpeta de grabaciones '{}'", baseFolder);
        return false;
    }

    const std::string stamp = core::SystemPaths::timestampName();
    sessionFolder_ = baseFolder + "/Grabación " + stamp;
    fs::create_directories(sessionFolder_, ec);
    if (ec) {
        core::Log::instance().errorFmt("No se pudo crear la sesión '{}'", sessionFolder_);
        return false;
    }

    sceneName_ = sceneName;
    frameCount_ = 0;
    recording_ = true;
    core::Log::instance().infoFmt("Grabación iniciada → {}", sessionFolder_);
    return true;
}

void FrameRecorder::captureFrame(const std::vector<u8>& rgba, i32 width, i32 height) {
    if (!recording_ || rgba.empty()) {
        return;
    }
    std::ostringstream name;
    name << "fotograma_" << (frameCount_ + 1) << ".bmp";
    const std::string path = sessionFolder_ + "/" + name.str();
    if (!BmpImage::writeBmp24(path, width, height, rgba.data())) {
        core::Log::instance().warnFmt("No se pudo guardar el fotograma {}", path);
        return;
    }
    ++frameCount_;
}

std::string FrameRecorder::endSession(f64 fps) {
    if (!recording_) {
        return sessionFolder_;
    }
    recording_ = false;

    std::ofstream manifest(sessionFolder_ + "/informacion.json", std::ios::trunc);
    if (manifest) {
        manifest << "{\n"
                 << "  \"motor\": \"UNIVERSAL ENGINE\",\n"
                 << "  \"escena\": \"" << sceneName_ << "\",\n"
                 << "  \"fotogramas\": " << frameCount_ << ",\n"
                 << "  \"fps\": " << fps << ",\n"
                 << "  \"formato\": \"secuencia de imagenes BMP\"\n"
                 << "}\n";
    }
    core::Log::instance().infoFmt("Grabación finalizada: {} fotogramas en {}", frameCount_,
                                  sessionFolder_);
    return sessionFolder_;
}

} // namespace ue::media