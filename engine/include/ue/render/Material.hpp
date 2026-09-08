#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Color.hpp"
#include "ue/render/Texture2D.hpp"

#include <memory>

namespace ue::render {

/// Material PBR simplificado (color base + brillo + textura).
class Material {
public:
    math::Color baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    f32 shininess = 48.0f;
    std::shared_ptr<const Texture2D> albedo; // textura opcional

    bool hasTexture() const { return albedo && albedo->isValid(); }
};

using MaterialPtr = std::shared_ptr<Material>;

} // namespace ue::render