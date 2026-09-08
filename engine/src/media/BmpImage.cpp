#include "ue/media/BmpImage.hpp"

#include <fstream>
#include <vector>

namespace ue::media {

bool BmpImage::writeBmp24(const std::string& path, i32 width, i32 height, const u8* rgba) {
    if (!rgba || width <= 0 || height <= 0) {
        return false;
    }

    const i32 bpp = 24;
    const i32 rowPaddedSize = ((width * bpp + 31) / 32) * 4;
    const u32 dataSize = static_cast<u32>(rowPaddedSize) * static_cast<u32>(height);
    const u32 fileSize = 54u + dataSize;

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }

    // Cabecera de archivo (BITMAPFILEHEADER)
    const u8 fileHeader[14] = {
        'B', 'M',
        static_cast<u8>(fileSize & 0xFF), static_cast<u8>((fileSize >> 8) & 0xFF),
        static_cast<u8>((fileSize >> 16) & 0xFF), static_cast<u8>((fileSize >> 24) & 0xFF),
        0, 0, 0, 0,
        54, 0, 0, 0,
    };

    // Cabecera de información (BITMAPINFOHEADER)
    const i32 ihSize = 40;
    const u8 infoHeader[40] = {
        static_cast<u8>(ihSize), 0, 0, 0,
        static_cast<u8>(width & 0xFF), static_cast<u8>((width >> 8) & 0xFF),
        static_cast<u8>((width >> 16) & 0xFF), static_cast<u8>((width >> 24) & 0xFF),
        static_cast<u8>(height & 0xFF), static_cast<u8>((height >> 8) & 0xFF),
        static_cast<u8>((height >> 16) & 0xFF), static_cast<u8>((height >> 24) & 0xFF),
        1, 0,                      // planos
        static_cast<u8>(bpp), 0,   // bits por píxel
        0, 0, 0, 0,                // sin compresión
        static_cast<u8>(dataSize & 0xFF), static_cast<u8>((dataSize >> 8) & 0xFF),
        static_cast<u8>((dataSize >> 16) & 0xFF), static_cast<u8>((dataSize >> 24) & 0xFF),
        0xB1, 0x0B, 0, 0,          // resolución X (2835 ppm)
        0xB1, 0x0B, 0, 0,          // resolución Y
        0, 0, 0, 0,                // colores usados
        0, 0, 0, 0,                // colores importantes
    };

    file.write(reinterpret_cast<const char*>(fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char*>(infoHeader), sizeof(infoHeader));

    std::vector<u8> row(static_cast<size_t>(rowPaddedSize), 0);
    for (i32 y = height - 1; y >= 0; --y) { // BMP guarda filas desde abajo
        const u8* src = rgba + static_cast<size_t>(y) * width * 4u;
        for (i32 x = 0; x < width; ++x) {
            row[static_cast<size_t>(x) * 3u + 0] = src[static_cast<size_t>(x) * 4u + 2]; // B
            row[static_cast<size_t>(x) * 3u + 1] = src[static_cast<size_t>(x) * 4u + 1]; // G
            row[static_cast<size_t>(x) * 3u + 2] = src[static_cast<size_t>(x) * 4u + 0]; // R
        }
        file.write(reinterpret_cast<const char*>(row.data()), row.size());
    }

    file.flush();
    return static_cast<bool>(file);
}

} // namespace ue::media