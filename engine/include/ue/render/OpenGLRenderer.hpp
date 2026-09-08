#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Color.hpp"
#include "ue/math/Matrix4.hpp"
#include "ue/render/IRenderer.hpp"
#include "ue/render/ShaderProgram.hpp"
#include "ue/render/Vertex.hpp"

#include <unordered_map>

namespace ue::render {

class Camera;
struct Lights;

/// Renderizador OpenGL 4.3 con rejilla de trabajo y FBO para el viewport.
class OpenGLRenderer final : public IRenderer {
public:
    OpenGLRenderer() = default;
    ~OpenGLRenderer() override;

    bool initialize(platform::IWindow& window) override;
    void shutdown() override;

    void setViewportSize(i32 width, i32 height) override;
    void setWireframe(bool enabled) override { wireframe_ = enabled; }

    void beginScene(const Camera& camera, const Lights& lights,
                    const math::Color& clearColor) override;
    void drawMeshObject(const scene::GameObject& object,
                        const scene::MeshComponent& mesh) override;
    void drawGrid(const Camera& camera) override;
    void drawAxes(const Camera& camera) override;
    void endScene() override;
    void presentToScreen() override;

    u32 viewportColorTexture() const override { return colorTex_; }
    i32 viewportWidth() const override  { return vpW_; }
    i32 viewportHeight() const override { return vpH_; }

    void readViewportRGBA(std::vector<u8>& rgba) const override;

    GpuMesh& meshCacheFor(scene::MeshComponent& mesh) override;

private:
    bool createViewportFramebuffer(i32 width, i32 height);
    void destroyViewportFramebuffer();
    MeshDataPtr buildGrid();
    MeshDataPtr buildAxes();

    ShaderProgram litProgram_;
    ShaderProgram unlitProgram_;

    MeshDataPtr gridData_;
    GpuMesh gridMesh_;
    MeshDataPtr axisData_;
    GpuMesh axisMesh_;

    u32 fbo_      = 0;
    u32 colorTex_ = 0;
    u32 depthRbo_ = 0;
    i32 vpW_ = 0;
    i32 vpH_ = 0;
    bool wireframe_ = false;

    Matrix4 view_;
    Matrix4 projection_;
    Vector3 cameraPosition_;
    Lights lights_;

    std::unordered_map<const scene::MeshComponent*, GpuMesh> meshCache_;
    std::unordered_map<const scene::MeshComponent*, core::u64> meshRevisions_;
};

} // namespace ue::render