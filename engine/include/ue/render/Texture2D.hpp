#pragma once

#include "ue/core/Types.hpp"
#include "ue/render/Gl.hpp"

#include <memory>

namespace ue::render {

/// Textura 2D residente en GPU.
class Texture2D {
public:
    Texture2D() = default;
    ~Texture2D();

    /// Crea la textura desde píxeles RGBA8.
    bool upload(i32 width, i32 height, const u8* rgbaData);

    void bind(u32 unit = 0) const;
    void release();

    u32 id() const { return texture_; }
    i32 width() const { return width_; }
    i32 height() const { return height_; }
    bool isValid() const { return texture_ != 0; }

    /// Textura a cuadros por defecto para materiales sin imagen.
    static std::shared_ptr<Texture2D> createCheckerboard();

    /// Carga desde un archivo BMP de 24 bits; nullptr si falla.
    static std::shared_ptr<Texture2D> loadFromBmp(const std::string& path);

private:
    u32 texture_ = 0;
    i32 width_ = 0;
    i32 height_ = 0;
};

} // namespace ue::render