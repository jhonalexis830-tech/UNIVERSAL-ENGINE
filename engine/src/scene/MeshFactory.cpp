#include "ue/scene/MeshFactory.hpp"

#include <cmath>
#include <map>

namespace ue::scene {

namespace {
    using render::MeshData;
    using render::MeshDataPtr;
    using render::Vertex;

    struct Builder {
        MeshData data;

        u32 addVertex(const math::Vector3& p, const math::Vector3& n,
                      f32 u = 0.0f, f32 v = 0.0f) {
            data.vertices.push_back({ p, n, { u, v } });
            return static_cast<u32>(data.vertices.size()) - 1u;
        }

        void quad(u32 a, u32 b, u32 c, u32 d) {
            const u32 idx[6] = { a, b, c, a, c, d };
            data.indices.insert(data.indices.end(), idx, idx + 6);
        }

        void triangle(u32 a, u32 b, u32 c) {
            data.indices.push_back(a);
            data.indices.push_back(b);
            data.indices.push_back(c);
        }

        MeshDataPtr finish() {
            return std::make_shared<MeshData>(std::move(data));
        }
    };

    // Tabla de colores internos para el marcador de ejes.
    void appendAxis(Builder& b, const math::Vector3& dir) {
        const u32 start = b.addVertex(math::Vector3::zero(), math::Vector3::up());
        const u32 end = b.addVertex(dir, dir.normalized());
        b.data.indices.push_back(start);
        b.data.indices.push_back(end);
    }
}

render::MeshDataPtr MeshFactory::cube(f32 size) {
    const f32 h = size * 0.5f;
    Builder b;

    // [normal, 4 esquinas] por cara, con UVs de 0..1.
    struct Face {
        math::Vector3 n;
        math::Vector3 c[4];
    };
    const Face faces[6] = {
        { {  1,  0,  0 }, { {  h, -h, -h }, {  h, -h,  h }, {  h,  h,  h }, {  h,  h, -h } } },
        { { -1,  0,  0 }, { { -h, -h,  h }, { -h, -h, -h }, { -h,  h, -h }, { -h,  h,  h } } },
        { {  0,  1,  0 }, { { -h,  h, -h }, { -h,  h,  h }, {  h,  h,  h }, {  h,  h, -h } } },
        { {  0, -1,  0 }, { { -h, -h,  h }, { -h, -h, -h }, {  h, -h, -h }, {  h, -h,  h } } },
        { {  0,  0,  1 }, { { -h, -h,  h }, { -h,  h,  h }, {  h,  h,  h }, {  h, -h,  h } } },
        { {  0,  0, -1 }, { {  h, -h, -h }, {  h,  h, -h }, { -h,  h, -h }, { -h, -h, -h } } },
    };

    const math::Vector2 uv[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

    for (const Face& face : faces) {
        u32 idx[4];
        for (int i = 0; i < 4; ++i) {
            idx[i] = b.addVertex(face.c[i], face.n, uv[i].x, uv[i].y);
        }
        b.quad(idx[0], idx[1], idx[2], idx[3]);
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::plane(f32 width, f32 depth, i32 segmentsW, i32 segmentsD) {
    Builder b;
    const f32 hw = width * 0.5f;
    const f32 hd = depth * 0.5f;
    const int gw = std::max(segmentsW, 1);
    const int gd = std::max(segmentsD, 1);

    std::vector<std::vector<u32>> grid(static_cast<size_t>(gd) + 1,
                                       std::vector<u32>(static_cast<size_t>(gw) + 1, 0));
    for (int j = 0; j <= gd; ++j) {
        for (int i = 0; i <= gw; ++i) {
            const f32 u = static_cast<f32>(i) / static_cast<f32>(gw);
            const f32 v = static_cast<f32>(j) / static_cast<f32>(gd);
            const math::Vector3 p(-hw + width * u, 0.0f, -hd + depth * v);
            grid[static_cast<size_t>(j)][static_cast<size_t>(i)] =
                b.addVertex(p, math::Vector3::up(), u, v);
        }
    }
    for (int j = 0; j < gd; ++j) {
        for (int i = 0; i < gw; ++i) {
            b.quad(grid[static_cast<size_t>(j)][static_cast<size_t>(i)],
                   grid[static_cast<size_t>(j)][static_cast<size_t>(i + 1)],
                   grid[static_cast<size_t>(j + 1)][static_cast<size_t>(i + 1)],
                   grid[static_cast<size_t>(j + 1)][static_cast<size_t>(i)]);
        }
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::sphere(f32 radius, i32 slices, i32 stacks) {
    Builder b;
    const int nslices = std::max(slices, 3);
    const int nstacks = std::max(stacks, 2);

    // Vértices de la malla latlong
    for (int j = 0; j <= nstacks; ++j) {
        const f32 phi = math::kPi * static_cast<f32>(j) / static_cast<f32>(nstacks); // 0..PI
        const f32 sinPhi = std::sin(phi);
        const f32 cosPhi = std::cos(phi);
        const f32 vv = static_cast<f32>(j) / static_cast<f32>(nstacks);
        for (int i = 0; i <= nslices; ++i) {
            const f32 theta = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(nslices);
            const f32 sinTheta = std::sin(theta);
            const f32 cosTheta = std::cos(theta);
            const math::Vector3 dir(cosTheta * sinPhi, cosPhi, sinTheta * sinPhi);
            b.addVertex(dir * radius, dir, static_cast<f32>(i) / static_cast<f32>(nslices), vv);
        }
    }
    for (int j = 0; j < nstacks; ++j) {
        for (int i = 0; i < nslices; ++i) {
            const u32 a = static_cast<u32>(j) * (nslices + 1) + i;
            const u32 b = a + nslices + 1;
            b.quad(a, b, b + 1, a + 1);
        }
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::cylinder(f32 radius, f32 height, i32 segments) {
    Builder b;
    const int n = std::max(segments, 3);
    const f32 hh = height * 0.5f;

    std::vector<u32> ring(n, 0);
    for (int i = 0; i < n; ++i) {
        const f32 t = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(n);
        const f32 c = std::cos(t), s = std::sin(t);
        const u32 bottom = b.addVertex({ radius * c, -hh, radius * s }, { c, 0.0f, s },
                                       static_cast<f32>(i) / static_cast<f32>(n), 0.0f);
        ring[static_cast<size_t>(i)] = bottom;
    }
    // Malla lateral (la fila superior se genera primero para enlazar bien).
    std::vector<u32> top(n, 0);
    for (int i = 0; i < n; ++i) {
        const f32 t = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(n);
        const f32 c = std::cos(t), s = std::sin(t);
        top[static_cast<size_t>(i)] = b.addVertex({ radius * c, hh, radius * s }, { c, 0.0f, s },
                                                  static_cast<f32>(i) / static_cast<f32>(n), 1.0f);
    }
    // Quads laterales
    const u32 centerTop = b.addVertex({ 0.0f, hh, 0.0f }, math::Vector3::up());
    const u32 centerBottom = b.addVertex({ 0.0f, -hh, 0.0f }, math::Vector3::down());
    for (int i = 0; i < n; ++i) {
        const int j = (i + 1) % n;
        b.quad(ring[static_cast<size_t>(i)], top[static_cast<size_t>(i)],
               top[static_cast<size_t>(j)], ring[static_cast<size_t>(j)]);
        b.triangle(top[static_cast<size_t>(i)], top[static_cast<size_t>(j)], centerTop);
        b.triangle(ring[static_cast<size_t>(j)], ring[static_cast<size_t>(i)], centerBottom);
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::cone(f32 radius, f32 height, i32 segments) {
    Builder b;
    const int n = std::max(segments, 3);
    const f32 hh = height * 0.5f;
    const f32 slope = radius / std::max(height, 1e-4f);

    std::vector<u32> ring(n, 0);
    for (int i = 0; i < n; ++i) {
        const f32 t = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(n);
        const f32 c = std::cos(t), s = std::sin(t);
        const f32 u = static_cast<f32>(i) / static_cast<f32>(n);
        ring[static_cast<size_t>(i)] = b.addVertex({ radius * c, -hh, radius * s },
                                                   { c, slope, s }, u, 0.0f);
    }
    const u32 centerBase = b.addVertex({ 0.0f, -hh, 0.0f }, math::Vector3::down());

    std::vector<u32> apex(n, 0);
    for (int i = 0; i < n; ++i) {
        const f32 t = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(n);
        const f32 c = std::cos(t), s = std::sin(t);
        const f32 u = static_cast<f32>(i) / static_cast<f32>(n);
        apex[static_cast<size_t>(i)] = b.addVertex({ 0.0f, hh, 0.0f }, { c, slope, s }, u, 1.0f);
    }

    for (int i = 0; i < n; ++i) {
        const int j = (i + 1) % n;
        b.triangle(ring[static_cast<size_t>(i)], apex[static_cast<size_t>(i)],
                   apex[static_cast<size_t>(j)]);
        b.triangle(ring[static_cast<size_t>(j)], ring[static_cast<size_t>(i)], centerBase);
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::capsule(f32 height, f32 radius, i32 segments, i32 rings) {
    Builder b;
    const int n = std::max(segments, 3);
    const int nr = std::max(rings, 2);
    const f32 cylinderH = std::max(height - radius * 2.0f, 0.001f) * 0.5f;

    // Genera un anillo de vértices con normal dada; devuelve sus índices.
    auto ring = [&](f32 y, f32 rad, const math::Vector3& normal, f32 baseV) -> std::vector<u32> {
        std::vector<u32> idx;
        idx.reserve(static_cast<size_t>(n) + 1);
        for (int i = 0; i <= n; ++i) {
            const f32 t = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(n);
            const f32 c = std::cos(t), s = std::sin(t);
            idx.push_back(b.addVertex({ rad * c, y, rad * s }, normal,
                                      static_cast<f32>(i) / static_cast<f32>(n), baseV));
        }
        return idx;
    };
    // Une dos anillos consecutivos en quads.
    auto bridge = [&](const std::vector<u32>& a, const std::vector<u32>& b2) {
        for (int i = 0; i < n; ++i) {
            b.quad(a[static_cast<size_t>(i)], b2[static_cast<size_t>(i)],
                   b2[static_cast<size_t>(i + 1)], a[static_cast<size_t>(i + 1)]);
        }
    };

    std::vector<std::vector<u32>> upper; // desde la unión hacia el polo
    std::vector<std::vector<u32>> lower;

    upper.push_back(ring(cylinderH, radius, math::Vector3(0.0f, 1.0f, 0.0f), 0.0f));
    lower.push_back(ring(-cylinderH, radius, math::Vector3(0.0f, -1.0f, 0.0f), 0.0f));
    for (int k = 1; k <= nr; ++k) {
        const f32 ang = math::degToRad(90.0f * static_cast<f32>(k) / static_cast<f32>(nr));
        const f32 some = std::sin(ang);
        const f32 rad = radius * some;
        upper.push_back(ring(cylinderH + radius * std::cos(ang), rad,
                             math::Vector3(0.0f, std::cos(ang), 0.0f), 0.0f));
        lower.push_back(ring(-cylinderH - radius * std::cos(ang), rad,
                             math::Vector3(0.0f, -std::cos(ang), 0.0f), 0.0f));
    }

    // Cilindro lateral: anillos con normales radiales.
    auto sideRing = [&](f32 y, f32 baseV) -> std::vector<u32> {
        std::vector<u32> idx;
        idx.reserve(static_cast<size_t>(n) + 1);
        for (int i = 0; i <= n; ++i) {
            const f32 t = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(n);
            const f32 c = std::cos(t), s = std::sin(t);
            idx.push_back(b.addVertex({ radius * c, y, radius * s }, { c, 0.0f, s },
                                      static_cast<f32>(i) / static_cast<f32>(n), baseV));
        }
        return idx;
    };
    const auto sideTop = sideRing(cylinderH, 1.0f);
    const auto sideBottom = sideRing(-cylinderH, 0.0f);
    bridge(sideBottom, sideTop);
    // Hemisferios: cada par de anillos consecutivos.
    for (int k = 0; k < nr; ++k) {
        bridge(upper[static_cast<size_t>(k)], upper[static_cast<size_t>(k + 1)]);
        bridge(lower[static_cast<size_t>(k)], lower[static_cast<size_t>(k + 1)]);
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::torus(f32 major, f32 minor, i32 majorSegments, i32 minorSegments) {
    Builder b;
    const int ns = std::max(majorSegments, 3);
    const int nt = std::max(minorSegments, 3);

    std::vector<std::vector<u32>> grid(static_cast<size_t>(ns) + 1,
                                       std::vector<u32>(static_cast<size_t>(nt) + 1, 0));
    for (int i = 0; i <= ns; ++i) {
        const f32 theta = math::kPi2 * static_cast<f32>(i) / static_cast<f32>(ns);
        const f32 cT = std::cos(theta), sT = std::sin(theta);
        for (int j = 0; j <= nt; ++j) {
            const f32 phi = math::kPi2 * static_cast<f32>(j) / static_cast<f32>(nt);
            const f32 cP = std::cos(phi), sP = std::sin(phi);
            const f32 arm = major + minor * cP;
            const math::Vector3 pos(arm * cT, minor * sP, arm * sT);
            const math::Vector3 normal(cP * cT, sP, cP * sT);
            b.addVertex(pos, normal,
                        static_cast<f32>(i) / static_cast<f32>(ns),
                        static_cast<f32>(j) / static_cast<f32>(nt));
            grid[static_cast<size_t>(i)][static_cast<size_t>(j)] =
                static_cast<u32>(b.data.vertices.size()) - 1u;
        }
    }
    for (int i = 0; i < ns; ++i) {
        for (int j = 0; j < nt; ++j) {
            const u32 a = grid[static_cast<size_t>(i)][static_cast<size_t>(j)];
            const u32 bb = grid[static_cast<size_t>(i)][static_cast<size_t>(j + 1)];
            const u32 c = grid[static_cast<size_t>(i + 1)][static_cast<size_t>(j + 1)];
            const u32 d = grid[static_cast<size_t>(i + 1)][static_cast<size_t>(j)];
            b.quad(a, bb, c, d);
        }
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::gridLines(f32 extent, i32 cells) {
    Builder b;
    const int n = std::max(cells, 1);
    const f32 step = (extent * 2.0f) / static_cast<f32>(n);

    for (int i = 0; i <= n; ++i) {
        const f32 coord = -extent + step * static_cast<f32>(i);
        b.data.vertices.push_back({ { coord, 0.0f, -extent }, math::Vector3::up(), { 0, 0 } });
        b.data.vertices.push_back({ { coord, 0.0f,  extent }, math::Vector3::up(), { 0, 0 } });
        b.data.vertices.push_back({ { -extent, 0.0f, coord }, math::Vector3::up(), { 0, 0 } });
        b.data.vertices.push_back({ {  extent, 0.0f, coord }, math::Vector3::up(), { 0, 0 } });
        b.data.indices.push_back(static_cast<u32>(b.data.vertices.size()) - 4u);
        b.data.indices.push_back(static_cast<u32>(b.data.vertices.size()) - 3u);
        b.data.indices.push_back(static_cast<u32>(b.data.vertices.size()) - 2u);
        b.data.indices.push_back(static_cast<u32>(b.data.vertices.size()) - 1u);
    }
    return b.finish();
}

render::MeshDataPtr MeshFactory::axisMarker(f32 length) {
    Builder b;
    appendAxis(b, math::Vector3(length, 0.0f, 0.0f));
    appendAxis(b, math::Vector3(0.0f, length, 0.0f));
    appendAxis(b, math::Vector3(0.0f, 0.0f, length));
    return b.finish();
}

} // namespace ue::scene