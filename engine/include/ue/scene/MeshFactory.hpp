#pragma once

#include "ue/core/Types.hpp"
#include "ue/render/Vertex.hpp"

namespace ue::scene {

/// Fábrica de mallas primitivas (estructuras pre-integradas del almacenamiento).
/// Todas se generan centradas en el origen y con normales por vértice.
class MeshFactory {
public:
    /// Cubo unitario de lado `size`.
    static render::MeshDataPtr cube(f32 size = 1.0f);

    /// Plano en el plano XZ (ancho X, profundidad Z).
    static render::MeshDataPtr plane(f32 width = 10.0f, f32 depth = 10.0f,
                                     i32 segmentsW = 1, i32 segmentsD = 1);

    /// Esfera UV (radio, cortes verticales y horizontales).
    static render::MeshDataPtr sphere(f32 radius = 0.5f, i32 slices = 24, i32 stacks = 12);

    /// Cilindro de radio y altura.
    static render::MeshDataPtr cylinder(f32 radius = 0.5f, f32 height = 1.0f, i32 segments = 24);

    /// Cono de radio base y altura.
    static render::MeshDataPtr cone(f32 radius = 0.5f, f32 height = 1.0f, i32 segments = 24);

    /// Cápsula (cuerpo cilíndrico + semiesferas) — útil para personajes.
    static render::MeshDataPtr capsule(f32 height = 2.0f, f32 radius = 0.5f,
                                       i32 segments = 24, i32 rings = 8);

    /// Toro de radios mayor y menor.
    static render::MeshDataPtr torus(f32 majorRadius = 0.7f, f32 minorRadius = 0.2f,
                                     i32 majorSegments = 24, i32 minorSegments = 12);

    /// Rejilla de línea (para el suelo del viewport). Se dibuja con GL_LINES.
    static render::MeshDataPtr gridLines(f32 extent = 10.0f, i32 cells = 10);

    /// Marco/vector de ejes XYZ (orígen → +X rojo, +Y verde, +Z azul).
    static render::MeshDataPtr axisMarker(f32 length = 1.0f);
};

} // namespace ue::scene