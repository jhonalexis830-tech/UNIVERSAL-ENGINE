#include "ue/render/Texture2D.hpp"

#include "ue/core/Log.hpp"
#include "ue/render/Vertex.hpp"

#include <fstream>
#include <vector>

namespace ue::render {

namespace {
    GlFunctions& gl() { return GlLoader::functions(); }
}

Texture2D::~Texture2D() {
    release();
}

bool Texture2D::upload(i32 width, i32 height, const u8* rgbaData) {
    GlFunctions& g = gl();
    if (!g.GlGenTextures) {
        return false;
    }
    if (texture_ == 0) {
        g.GlGenTextures(1, &texture_);
    }

    g.GlBindTexture(GL_TEXTURE_2D, texture_);
    g.GlTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA8), width, height, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_REPEAT));
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_REPEAT));
    g.GlBindTexture(GL_TEXTURE_2D, 0);

    width_ = width;
    height_ = height;
    return true;
}

void Texture2D::bind(u32 unit) const {
    GlFunctions& g = gl();
    if (texture_ == 0) {
        return;
    }
    g.GlActiveTexture(GL_TEXTURE0 + unit);
    g.GlBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture2D::release() {
    GlFunctions& g = gl();
    if (texture_ != 0) {
        g.GlDeleteTextures(1, &texture_);
        texture_ = 0;
    }
    width_ = height_ = 0;
}

std::shared_ptr<Texture2D> Texture2D::createCheckerboard() {
    constexpr i32 size = 16;
    std::vector<u8> rgba(static_cast<size_t>(size) * size * 4u);
    for (i32 y = 0; y < size; ++y) {
        for (i32 x = 0; x < size; ++x) {
            const bool light = ((x / 2) + (y / 2)) % 2 == 0;
            const u8 v = light ? 200 : 90;
            rgba[(static_cast<size_t>(y) * size + x) * 4u + 0] = v;
            rgba[(static_cast<size_t>(y) * size + x) * 4u + 1] = v;
            rgba[(static_cast<size_t>(y) * size + x) * 4u + 2] = v;
            rgba[(static_cast<size_t>(y) * size + x) * 4u + 3] = 255;
        }
    }
    auto tex = std::make_shared<Texture2D>();
    if (tex->upload(size, size, rgba.data())) {
        return tex;
    }
    return nullptr;
}

std::shared_ptr<Texture2D> Texture2D::loadFromBmp(const std::string& path) {
    // BMP de 24 bits sin compresión.
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        ue::core::Log::instance().warnFmt("No se pudo abrir la textura '{}'", path);
        return nullptr;
    }

    struct BmpHeader {
        u8 signature[2];
        u32 fileSize;
        u16 reserved1;
        u16 reserved2;
        u32 dataOffset;
        u32 dibSize;
        i32 width;
        i32 height;
        u16 planes;
        u16 bitsPerPixel;
    };

    BmpHeader header{};
    file.read(reinterpret_cast<char*>(&header), sizeof(BmpHeader));
    if (!file || header.signature[0] != 'B' || header.signature[1] != 'M' ||
        header.bitsPerPixel != 24 || header.width <= 0 || header.height == 0) {
        ue::core::Log::instance().warnFmt("Formato BMP no soportado: {}", path);
        return nullptr;
    }

    const bool topDown = header.height < 0;
    const i32 h = (header.height < 0) ? -header.height : header.height;
    const i32 rowSize = ((header.width * 3 + 3) & ~3u); // alineado a 4 bytes

    std::vector<u8> bgr(static_cast<size_t>(rowSize) * h);
    file.seekg(header.dataOffset);
    file.read(reinterpret_cast<char*>(bgr.data()),
              static_cast<std::streamsize>(bgr.size()));

    std::vector<u8> rgba(static_cast<size_t>(header.width) * h * 4u);
    for (i32 y = 0; y < h; ++y) {
        const i32 srcRow = topDown ? (h - 1 - y) : y;
        const i32 dstRow = h - 1 - y; // giramos para que arriba sea y=0 en textura
        for (i32 x = 0; x < header.width; ++x) {
            const size_t si = static_cast<size_t>(srcRow) * rowSize + static_cast<size_t>(x) * 3u;
            const size_t di = (static_cast<size_t>(dstRow) * header.width + x) * 4u;
            rgba[di + 0] = bgr[si + 2];
            rgba[di + 1] = bgr[si + 1];
            rgba[di + 2] = bgr[si + 0];
            rgba[di + 3] = 255;
        }
    }

    auto tex = std::make_shared<Texture2D>();
    if (tex->upload(header.width, h, rgba.data())) {
        ue::core::Log::instance().infoFmt("Textura cargada: {}", path);
        return tex;
    }
    return nullptr;
}

} // namespace ue::render