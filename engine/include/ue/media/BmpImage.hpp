#pragma once

#include "ue/core/Types.hpp"

#include <string>

namespace ue::media {

/// Escritor de imágenes BMP de 24 bits. Indispensable para las capturas de
/// pantalla y la grabación de secuencias de video (sin dependencias externas).
class BmpImage {
public:
    /// Escribe una imagen RGBA8 en un BMP de 24 bits (arriba-abajo).
    /// `rgba` debe contener width*height*4 bytes; la primera fila es la superior.
    static bool writeBmp24(const std::string& path, i32 width, i32 height,
                           const u8* rgba);
};

} // namespace ue::media