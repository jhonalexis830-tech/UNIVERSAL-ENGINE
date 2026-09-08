#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Vector2.hpp"
#include "ue/math/Vector3.hpp"

#include <memory>
#include <vector>

namespace ue::render {

/// Vértice de una malla.
struct Vertex {
    math::Vector3 position;
    math::Vector3 normal;
    math::Vector2 uv;
};

/// Malla en memoria de la CPU (Datos de malla).
/// Las subpartes del motor (render, física, animación) trabajan sobre copias
/// compartidas; los cambios se re-suben a la GPU bajo demanda.
class MeshData {
public:
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    u32 vertexCount() const { return static_cast<u32>(vertices.size()); }
    u32 triangleCount() const { return static_cast<u32>(indices.size() / 3u); }
    u32 indexCount() const { return static_cast<u32>(indices.size()); }

    void clear() {
        vertices.clear();
        indices.clear();
    }

    bool isEmpty() const { return vertices.empty() || indices.empty(); }

    /// Recalcula las normales por-vertex promediando las normales de cara.
    void recomputeNormals();
};

using MeshDataPtr = std::shared_ptr<MeshData>;

} // namespace ue::render