#pragma once

#include "ue/core/Types.hpp"
#include "ue/render/Vertex.hpp"

namespace ue::render {

/// Malla residente en la GPU (VAO/VBO/EBO). Soporta re-subida dinámica
/// cuando el usuario edita vértices en el editor.
class GpuMesh {
public:
    GpuMesh() = default;
    ~GpuMesh();

    /// Sube (o vuelve a subir) los datos de la malla a la GPU.
    bool upload(const MeshData& data);
    void release();

    /// mode: GL_TRIANGLES (predeterminado) o GL_LINES (rejillas/asistentes).
    void draw(u32 mode = 0x0004 /*GL_TRIANGLES*/) const;

    /// Re-subida de vértices después de una edición (optimizado).
    void updateVertices(const MeshData& data);

    u32 vertexArray() const { return vao_; }
    b32 isReady() const { return vao_ != 0; }

private:
    u32 vao_ = 0;
    u32 vbo_ = 0;
    u32 ebo_ = 0;
    u32 indexCount_ = 0;
    u32 vertexCount_ = 0;
    b32 indicesDirty_ = false;
};

} // namespace ue::render