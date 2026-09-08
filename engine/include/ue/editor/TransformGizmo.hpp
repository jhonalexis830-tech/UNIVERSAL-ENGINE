#pragma once

#include "ue/core/Types.hpp"
#include "ue/editor/ToolType.hpp"
#include "ue/math/Matrix4.hpp"
#include "ue/math/Vector2.hpp"
#include "ue/math/Vector3.hpp"

struct ImDrawList;

namespace ue::scene {
class Scene;
class GameObject;
}

namespace ue::render {
class Camera;
}

namespace ue::editor {

/// Gizmo de transformación 3D (estilo Blender/Unity):
///  - Mover  : flechas por eje (X rojo, Y verde, Z azul).
///  - Rotar  : anillos por eje en pantalla.
///  - Escalar: cuadros por eje + centro para escala uniforme.
///  - Edición de malla: mango por vértice (punto azul/naranja).
class TransformGizmo {
public:
    void update(Tool tool, scene::Scene& scene, const render::Camera& camera,
                math::Vector2 mousePx, math::Vector2 viewportSizePx,
                bool leftButtonDown, bool leftButtonPressed, bool viewportHovered,
                scene::GameObject* selection, i32& selectedVertex,
                bool& meshEdited);

    void draw(ImDrawList& drawList, const render::Camera& camera,
              math::Vector2 viewportPosPx, math::Vector2 viewportSizePx,
              const scene::GameObject* selection, Tool tool) const;

    bool isDragging() const { return dragging_; }
    int activeAxis() const { return activeAxis_; }

private:
    math::Vector2 projectToScreen(const math::Vector3& world, const math::Matrix4& viewProj,
                                  math::Vector2 viewportPosPx, math::Vector2 viewportSizePx,
                                  bool& behind) const;

    int activeAxis_ = -1;      // 0=X, 1=Y, 2=Z, 3=centro
    bool dragging_ = false;
    math::Vector3 dragStart_;
    f32 startAngle_ = 0.0f;
    math::Vector2 startMouse_;
    math::Vector3 planeCenter_;
    math::Vector3 planeNormal_;

    // Edición de vértices
    int hotVertex_ = -1;
    math::Vector3 vertexLastHit_;
};

} // namespace ue::editor