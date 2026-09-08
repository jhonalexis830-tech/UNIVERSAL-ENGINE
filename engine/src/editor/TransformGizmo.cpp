#include "ue/editor/TransformGizmo.hpp"

#include "ue/editor/Picker.hpp"
#include "ue/math/Color.hpp"
#include "ue/math/Math.hpp"
#include "ue/render/Camera.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <imgui.h>

#include <algorithm>
#include <cmath>

namespace ue::editor {

namespace {
    constexpr f32 kAxisPickRadius = 14.0f;   // píxeles
    constexpr f32 kHandleRadius   = 6.0f;
    constexpr int  kRingSamples   = 64;

    math::Vector3 axisDir(int axis) {
        switch (axis) {
            case 0: return math::Vector3::right();
            case 1: return math::Vector3::up();
            case 2: return math::Vector3::forward();
            default: return math::Vector3::one();
        }
    }

    // Intersección rayo-plano en forma de punto/vector normal.
    bool rayPlane(const Ray& ray, const math::Vector3& planePoint,
                  const math::Vector3& planeNormal, math::Vector3& outPoint) {
        const f32 denom = ray.direction.dot(planeNormal);
        if (std::fabs(denom) < 1e-5f) {
            return false;
        }
        const f32 t = (planePoint - ray.origin).dot(planeNormal) / denom;
        if (t < 0.0f) {
            return false;
        }
        outPoint = ray.origin + ray.direction * t;
        return true;
    }

    ImU32 axisColor(int axis, bool highlighted) {
        if (highlighted) {
            return IM_COL32(255, 255, 255, 255);
        }
        switch (axis) {
            case 0: return IM_COL32(224, 82, 82, 255);    // X rojo
            case 1: return IM_COL32(95, 191, 95, 255);    // Y verde
            case 2: return IM_COL32(79, 163, 224, 255);   // Z azul
            default: return IM_COL32(150, 190, 240, 255); // centro
        }
    }
}

math::Vector2 TransformGizmo::projectToScreen(const math::Vector3& world,
                                              const math::Matrix4& viewProj,
                                              math::Vector2 viewportPosPx,
                                              math::Vector2 viewportSizePx,
                                              bool& behind) const {
    const math::Vector3 clip = viewProj.transformPoint(world);
    const f32 w = clip.z != 0.0f ? clip.z : 1e-6f; // perspectiva: usar z como w (sin vista homogénea real)
    float ndcX = clip.x / w;
    float ndcY = clip.y / w;
    behind = w < 0.0f;
    ndcX = std::clamp(ndcX, -1.0f, 1.0f);
    ndcY = std::clamp(ndcY, -1.0f, 1.0f);
    return math::Vector2(viewportPosPx.x + (ndcX * 0.5f + 0.5f) * viewportSizePx.x,
                         viewportPosPx.y + (0.5f - ndcY * 0.5f) * viewportSizePx.y);
}

void TransformGizmo::update(Tool tool, scene::Scene& scene, const render::Camera& camera,
                            math::Vector2 mousePx, math::Vector2 viewportSizePx,
                            bool leftButtonDown, bool leftButtonPressed, bool viewportHovered,
                            scene::GameObject* selection, i32& selectedVertex,
                            bool& meshEdited) {
    const bool toolHasGizmo = (tool == Tool::Mover || tool == Tool::Rotar || tool == Tool::Escalar);
    if (!selection || (!toolHasGizmo && tool != Tool::EditarMalla)) {
        dragging_ = false;
        hotVertex_ = -1;
        return;
    }

    const f32 aspect = viewportSizePx.x / std::max(viewportSizePx.y, 1.0f);
    const math::Matrix4 viewProj = camera.projectionMatrix(aspect) * camera.viewMatrix();

    // ---------- EDICIÓN DE MALLAS (polígonos con el cursor) -------------
    if (tool == Tool::EditarMalla) {
        const auto mesh = selection->getComponent<scene::MeshComponent>();
        if (!mesh || !mesh->data() || mesh->data()->vertices.empty()) {
            return;
        }
        render::MeshData& data = *mesh->data();
        const math::Matrix4 model = selection->worldMatrix();
        const math::Matrix4 invModel = model.inverse();
        const math::Vector3& camFwd = camera.forward();

        // Selección del vértice más cercano al hacer clic.
        if (leftButtonPressed && viewportHovered) {
            f32 bestDist = kHandleRadius * 3.0f;
            i32 bestVertex = -1;
            for (i32 i = 0; i < static_cast<i32>(data.vertices.size()); ++i) {
                bool behind = false;
                const math::Vector2 screen =
                    projectToScreen(model.transformPoint(data.vertices[static_cast<size_t>(i)].position),
                                    viewProj, math::Vector2::zero(), viewportSizePx, behind);
                if (behind) {
                    continue;
                }
                const f32 d = (screen - mousePx).length();
                if (d < bestDist) {
                    bestDist = d;
                    bestVertex = i;
                }
            }
            selectedVertex = bestVertex;
            hotVertex_ = bestVertex;
            if (selectedVertex >= 0) {
                const math::Vector3 worldP = model.transformPoint(
                    data.vertices[static_cast<size_t>(selectedVertex)].position);
                rayPlane(Picker::cameraRay(camera, mousePx, viewportSizePx), worldP, camFwd,
                         vertexLastHit_);
            }
        }

        // Arrastre del vértice seleccionado en el plano paralelo a la cámara.
        if (leftButtonDown && selectedVertex >= 0) {
            const math::Vector3 worldP = model.transformPoint(
                data.vertices[static_cast<size_t>(selectedVertex)].position);
            math::Vector3 hit;
            if (rayPlane(Picker::cameraRay(camera, mousePx, viewportSizePx), worldP, camFwd, hit)) {
                const math::Vector3 worldDelta = hit - vertexLastHit_;
                if (worldDelta.lengthSq() > 1e-9f) {
                    const math::Vector3 localDelta = invModel.transformVector(worldDelta);
                    data.vertices[static_cast<size_t>(selectedVertex)].position += localDelta;
                    mesh->markEdited();
                    meshEdited = true;
                    vertexLastHit_ = hit;
                }
            }
        }
        return;
    }

    // ---------- GIZMO DE TRANSFORMACIÓN -------------------------------
    const math::Vector3 center = selection->worldPosition();
    const Ray ray = Picker::cameraRay(camera, mousePx, viewportSizePx);

    auto toScreen = [&](const math::Vector3& p) {
        bool behind = false;
        return projectToScreen(p, viewProj, math::Vector2::zero(), viewportSizePx, behind);
    };
    const math::Vector2 centerScreen = toScreen(center);

    // Escala de píxeles → mundo para el tamaño del gizmo.
    const math::Vector2 oneUp = toScreen(center + math::Vector3::up());
    f32 pxPerUnit = std::max((centerScreen - oneUp).length(), 1e-3f);
    const f32 scaleLen = std::clamp(1.4f * 90.0f / pxPerUnit, 0.4f, 12.0f);

    if (!dragging_) {
        hotVertex_ = -1;

        if (viewportHovered && leftButtonPressed) {
            // ¿Sobre qué eje estamos?
            int hit = -1;
            if (tool == Tool::Mover) {
                for (int a = 0; a < 3; ++a) {
                    const math::Vector2 end = toScreen(center + axisDir(a) * scaleLen);
                    if ((mousePx - end).length() < kAxisPickRadius) { hit = a; break; }
                }
            } else if (tool == Tool::Escalar) {
                for (int a = 0; a < 3; ++a) {
                    const math::Vector2 end = toScreen(center + axisDir(a) * scaleLen);
                    if ((mousePx - end).length() < kAxisPickRadius) { hit = a; break; }
                }
                if (hit == -1 && (mousePx - centerScreen).length() < kHandleRadius * 1.8f) {
                    hit = 3; // escala uniforme
                }
            } else if (tool == Tool::Rotar) {
                // Cerca de un anillo: elegir el eje por distancia al círculo proyectado.
                f32 bestRing = kHandleRadius * 2.0f + 10.0f;
                for (int a = 0; a < 3; ++a) {
                    const math::Vector3 u = (a == 0) ? math::Vector3::up()
                                                     : ((a == 1) ? math::Vector3::forward()
                                                                 : math::Vector3::right());
                    const math::Vector3 v = axisDir(a).cross(u).normalized();
                    const f32 ringWorld = 90.0f / pxPerUnit;
                    f32 bestDist = 1e9f;
                    for (int i = 0; i < kRingSamples; ++i) {
                        const f32 th = math::kPi2 * static_cast<f32>(i) / kRingSamples;
                        const math::Vector2 p = toScreen(center + (u * std::cos(th) + v * std::sin(th)) * ringWorld);
                        bestDist = std::min(bestDist, (p - mousePx).length());
                    }
                    if (bestDist < bestRing) {
                        bestRing = bestDist;
                        hit = a;
                    }
                }
            }

            if (hit >= 0) {
                dragging_ = true;
                activeAxis_ = hit;
                startMouse_ = mousePx;

                if (tool == Tool::Mover) {
                    dragStart_ = selection->transform.position;
                    planeCenter_ = center;
                    planeNormal_ = camera.forward();
                } else if (tool == Tool::Rotar) {
                    dragStart_ = selection->transform.rotationDegrees;
                    planeCenter_ = center;
                    planeNormal_ = axisDir(hit);
                    math::Vector3 hitPoint;
                    if (rayPlane(ray, planeCenter_, planeNormal_, hitPoint)) {
                        const math::Vector3 v = hitPoint - planeCenter_;
                        if (hit == 0) startAngle_ = std::atan2(v.z, v.y);
                        else if (hit == 1) startAngle_ = std::atan2(v.x, v.z);
                        else startAngle_ = std::atan2(v.y, v.x);
                    } else {
                        startAngle_ = 0.0f;
                    }
                } else if (tool == Tool::Escalar) {
                    dragStart_ = selection->transform.scale;
                    planeCenter_ = center;
                }
            }
        }
    }

    if (dragging_) {
        if (!leftButtonDown) {
            dragging_ = false;
            activeAxis_ = -1;
            return;
        }

        if (tool == Tool::Mover) {
            const math::Vector3 dir = axisDir(activeAxis_);
            math::Vector3 hit;
            if (rayPlane(ray, planeCenter_, planeNormal_, hit)) {
                const f32 along = (hit - planeCenter_).dot(dir);
                selection->transform.position = dragStart_ + dir * along;
            }
        } else if (tool == Tool::Rotar) {
            // Ángulo alrededor del eje en su plano.
            math::Vector3 hit;
            if (rayPlane(ray, planeCenter_, planeNormal_, hit)) {
                const math::Vector3 v = hit - planeCenter_;
                f32 angle;
                if (activeAxis_ == 0) {
                    angle = std::atan2(v.z, v.y);
                } else if (activeAxis_ == 1) {
                    angle = std::atan2(v.x, v.z);
                } else {
                    angle = std::atan2(v.y, v.x);
                }
                f32 deltaDeg = math::radToDeg(angle - startAngle_);
                deltaDeg = std::fmod(deltaDeg + 360.0f, 360.0f);
                if (deltaDeg > 180.0f) deltaDeg -= 360.0f;
                math::Vector3 e = dragStart_;
                if (activeAxis_ == 0) e.x = std::fmod(dragStart_.x + deltaDeg, 360.0f);
                if (activeAxis_ == 1) e.y = std::fmod(dragStart_.y + deltaDeg, 360.0f);
                if (activeAxis_ == 2) e.z = std::fmod(dragStart_.z + deltaDeg, 360.0f);
                selection->transform.rotationDegrees = e;
            }
        } else if (tool == Tool::Escalar) {
            if (activeAxis_ == 3) {
                // Escala uniforme por arrastre vertical.
                const f32 dy = startMouse_.y - mousePx.y;
                const f32 factor = (dy == 0.0f) ? 1.0f : (1.0f + dy * 0.008f);
                selection->transform.scale = dragStart_ * std::max(factor, 0.02f);
            } else {
                const math::Vector3 dir = axisDir(activeAxis_);
                const math::Vector2 endScreen = toScreen(center + dir * scaleLen);
                const math::Vector2 axisScreen = (endScreen - centerScreen).normalized();
                const f32 startProj = (startMouse_ - centerScreen).dot(axisScreen) + pxPerUnit;
                const f32 nowProj = (mousePx - centerScreen).dot(axisScreen) + pxPerUnit;
                if (startProj > 0.0f) {
                    f32 ratio = nowProj / startProj;
                    ratio = std::clamp(ratio, 0.02f, 50.0f);
                    math::Vector3 s = dragStart_;
                    if (activeAxis_ == 0) s.x = dragStart_.x * ratio;
                    if (activeAxis_ == 1) s.y = dragStart_.y * ratio;
                    if (activeAxis_ == 2) s.z = dragStart_.z * ratio;
                    selection->transform.scale = s;
                }
            }
        }
        (void)scene;
    }
}

void TransformGizmo::draw(ImDrawList& drawList, const render::Camera& camera,
                          math::Vector2 viewportPosPx, math::Vector2 viewportSizePx,
                          const scene::GameObject* selection, Tool tool) const {
    if (!selection) {
        return;
    }
    const bool drawMove = (tool == Tool::Mover);
    const bool drawScale = (tool == Tool::Escalar);
    const bool drawRotate = (tool == Tool::Rotar);
    const bool drawVerts = (tool == Tool::EditarMalla);

    const f32 aspect = viewportSizePx.x / std::max(viewportSizePx.y, 1.0f);
    const math::Matrix4 viewProj = camera.projectionMatrix(aspect) * camera.viewMatrix();
    const math::Vector3 center = selection->worldPosition();

    auto toScreen = [&](const math::Vector3& p) {
        bool behind = false;
        return projectToScreen(p, viewProj, viewportPosPx, viewportSizePx, behind);
    };
    const math::Vector2 centerScreen = toScreen(center);
    const math::Vector2 oneUp = toScreen(center + math::Vector3::up());
    f32 pxPerUnit = std::max((centerScreen - oneUp).length(), 1e-3f);
    const f32 scaleLen = std::clamp(1.4f * 90.0f / pxPerUnit, 0.4f, 12.0f);

    // Centro
    drawList.AddCircleFilled(ImVec2(centerScreen.x, centerScreen.y), 4.0f,
                             IM_COL32(200, 220, 255, 255));

    if (drawVerts) {
        const auto mesh = selection->getComponent<scene::MeshComponent>();
        if (!mesh || !mesh->data()) {
            return;
        }
        const math::Matrix4 model = selection->worldMatrix();
        const render::MeshData& data = *mesh->data();

        // Aristas de la malla (vista de estructura).
        for (size_t i = 0; i + 2 < data.indices.size(); i += 3) {
            const auto& vi = data.vertices[data.indices[i]].position;
            const auto& vj = data.vertices[data.indices[i + 1]].position;
            const auto& vk = data.vertices[data.indices[i + 2]].position;
            const math::Vector2 a = toScreen(model.transformPoint(vi));
            const math::Vector2 b = toScreen(model.transformPoint(vj));
            const math::Vector2 c = toScreen(model.transformPoint(vk));
            drawList.AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(90, 160, 235, 120));
            drawList.AddLine(ImVec2(b.x, b.y), ImVec2(c.x, c.y), IM_COL32(90, 160, 235, 120));
            drawList.AddLine(ImVec2(c.x, c.y), ImVec2(a.x, a.y), IM_COL32(90, 160, 235, 120));
        }

        // Mangos por vértice.
        for (size_t i = 0; i < data.vertices.size(); ++i) {
            const bool selected = (static_cast<i32>(i) == hotVertex_);
            const math::Vector2 sp = toScreen(model.transformPoint(data.vertices[i].position));
            const ImVec2 p(sp.x, sp.y);
            ImU32 col = selected ? IM_COL32(255, 170, 60, 255)
                                 : IM_COL32(120, 180, 255, 220);
            drawList.AddCircleFilled(p, selected ? 6.0f : 4.0f, col);
        }
        return;
    }

    if (drawMove || drawScale) {
        for (int a = 0; a < 3; ++a) {
            const ImU32 col = axisColor(a, activeAxis_ == a && dragging_);
            const math::Vector2 end = toScreen(center + axisDir(a) * scaleLen);
            drawList.AddLine(ImVec2(centerScreen.x, centerScreen.y),
                             ImVec2(end.x, end.y), col, 2.5f);
            if (drawMove) {
                drawList.AddCircleFilled(ImVec2(end.x, end.y), 6.0f, col);
            } else {
                const f32 half = 5.0f;
                drawList.AddRectFilled(ImVec2(end.x - half, end.y - half),
                                       ImVec2(end.x + half, end.y + half), col);
            }
        }
        if (drawScale && activeAxis_ == 3 && dragging_) {
            drawList.AddCircleFilled(ImVec2(centerScreen.x, centerScreen.y), 7.0f,
                                     IM_COL32(255, 255, 255, 255));
        }
    } else if (drawRotate) {
        const f32 ringWorld = 90.0f / pxPerUnit;
        for (int aIdx = 0; aIdx < 3; ++aIdx) {
            const int a = aIdx;
            const math::Vector3 u = (a == 0) ? math::Vector3::up()
                                             : ((a == 1) ? math::Vector3::forward()
                                                         : math::Vector3::right());
            const math::Vector3 v = axisDir(a).cross(u).normalized();
            const ImU32 col = axisColor(a, activeAxis_ == a && dragging_);
            ImVec2 prev;
            bool have = false;
            for (int i = 0; i <= kRingSamples; ++i) {
                const f32 th = math::kPi2 * static_cast<f32>(i) / kRingSamples;
                const math::Vector2 p = toScreen(center + (u * std::cos(th) + v * std::sin(th)) * ringWorld);
                const ImVec2 cur(p.x, p.y);
                if (have) {
                    drawList.AddLine(prev, cur, col, 2.0f);
                }
                prev = cur;
                have = true;
            }
        }
    }
}

} // namespace ue::editor