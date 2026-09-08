#include "ue/render/GpuMesh.hpp"

#include "ue/render/Gl.hpp"

#include <cstddef>
#include <cstring>

namespace ue::render {

using namespace ue::render;

GpuMesh::~GpuMesh() {
    release();
}

bool GpuMesh::upload(const MeshData& data) {
    GlFunctions& g = GlLoader::functions();
    if (!g.GlGenVertexArrays || data.isEmpty()) {
        return false;
    }

    if (vao_ == 0) {
        g.GlGenVertexArrays(1, &vao_);
        g.GlGenBuffers(1, &vbo_);
        g.GlGenBuffers(1, &ebo_);
    }

    g.GlBindVertexArray(vao_);
    g.GlBindBuffer(GL_ARRAY_BUFFER, vbo_);
    g.GlBufferData(GL_ARRAY_BUFFER,
                   static_cast<GLsizeiptr>(data.vertices.size() * sizeof(Vertex)),
                   data.vertices.data(), GL_DYNAMIC_DRAW);

    g.GlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    g.GlBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   static_cast<GLsizeiptr>(data.indices.size() * sizeof(u32)),
                   data.indices.data(), GL_STATIC_DRAW);

    constexpr u32 kStride = sizeof(Vertex);
    g.GlEnableVertexAttribArray(0);
    g.GlVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, kStride,
                            reinterpret_cast<const GLvoid*>(offsetof(Vertex, position)));
    g.GlEnableVertexAttribArray(1);
    g.GlVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, kStride,
                            reinterpret_cast<const GLvoid*>(offsetof(Vertex, normal)));
    g.GlEnableVertexAttribArray(2);
    g.GlVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, kStride,
                            reinterpret_cast<const GLvoid*>(offsetof(Vertex, uv)));

    g.GlBindVertexArray(0);

    indexCount_ = data.indexCount();
    vertexCount_ = data.vertexCount();
    indicesDirty_ = true;
    return true;
}

void GpuMesh::release() {
    GlFunctions& g = GlLoader::functions();
    if (vao_ != 0) {
        g.GlDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (vbo_ != 0) {
        g.GlDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (ebo_ != 0) {
        g.GlDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    indexCount_ = 0;
    vertexCount_ = 0;
}

void GpuMesh::updateVertices(const MeshData& data) {
    GlFunctions& g = GlLoader::functions();
    if (vao_ == 0) {
        upload(data);
        return;
    }
    if (data.indices.size() != indexCount_) {
        upload(data);
        return;
    }
    g.GlBindBuffer(GL_ARRAY_BUFFER, vbo_);
    g.GlBufferData(GL_ARRAY_BUFFER,
                   static_cast<GLsizeiptr>(data.vertices.size() * sizeof(Vertex)),
                   data.vertices.data(), GL_DYNAMIC_DRAW);
    g.GlBindBuffer(GL_ARRAY_BUFFER, 0);
    vertexCount_ = data.vertexCount();
    indicesDirty_ = true;
}

void GpuMesh::draw(u32 mode) const {
    GlFunctions& g = GlLoader::functions();
    if (indexCount_ == 0) {
        return;
    }
    g.GlBindVertexArray(vao_);
    g.GlDrawElements(mode, static_cast<GLsizei>(indexCount_),
                     GL_UNSIGNED_INT, nullptr);
    g.GlBindVertexArray(0);
}

} // namespace ue::render