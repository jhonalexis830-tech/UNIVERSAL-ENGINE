#include "ue/render/Vertex.hpp"

namespace ue::render {

void MeshData::recomputeNormals() {
    for (auto& v : vertices) {
        v.normal = math::Vector3::zero();
    }

    for (u32 i = 0; i + 2 < indices.size(); i += 3) {
        const u32 i0 = indices[i];
        const u32 i1 = indices[i + 1];
        const u32 i2 = indices[i + 2];
        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) {
            continue;
        }
        const math::Vector3 a = vertices[i0].position;
        const math::Vector3 b = vertices[i1].position;
        const math::Vector3 c = vertices[i2].position;
        const math::Vector3 n = (b - a).cross(c - a).normalized();
        vertices[i0].normal += n;
        vertices[i1].normal += n;
        vertices[i2].normal += n;
    }

    for (auto& v : vertices) {
        v.normal = v.normal.normalized();
    }
}

} // namespace ue::render