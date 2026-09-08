#include "ue/render/OpenGLRenderer.hpp"

#include "ue/core/Log.hpp"
#include "ue/math/Vector3.hpp"
#include "ue/platform/IWindow.hpp"
#include "ue/render/Camera.hpp"
#include "ue/render/Gl.hpp"
#include "ue/render/ShaderSources.hpp"
#include "ue/scene/GameObject.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <cmath>

namespace ue::render {

namespace {
    GlFunctions& gl() { return GlLoader::functions(); }
}

OpenGLRenderer::~OpenGLRenderer() {
    shutdown();
}

bool OpenGLRenderer::initialize(platform::IWindow& window) {
    if (!GlLoader::load(window.glProcLoader())) {
        core::Log::instance().error("No se pudieron cargar las funciones de OpenGL.");
        return false;
    }
    GlFunctions& g = gl();

    const auto* version = g.GlVersion(GL_VERSION);
    core::Log::instance().infoFmt("Motor gráfico iniciado — OpenGL {}",
                                  version ? reinterpret_cast<const char*>(version) : "(desconocido)");

    if (!litProgram_.build(ShaderSources::kVertexLit, ShaderSources::kFragmentLit)) {
        return false;
    }
    if (!unlitProgram_.build(ShaderSources::kVertexUnlit, ShaderSources::kFragmentUnlit)) {
        return false;
    }

    g.GlEnable(GL_DEPTH_TEST);
    g.GlDepthFunc(GL_LEQUAL);
    g.GlEnable(GL_BLEND);
    g.GlBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g.GlDisable(GL_CULL_FACE); // mallas editadas pueden tener caras invertidas

    gridData_ = buildGrid();
    gridMesh_.upload(*gridData_);
    axisData_ = buildAxes();
    axisMesh_.upload(*axisData_);
    core::Log::instance().info("El VIEWPORT está listo para trabajar.");
    return true;
}

MeshDataPtr OpenGLRenderer::buildAxes() {
    auto data = std::make_shared<MeshData>();
    // Tres segmentos desde el origen: +X, +Y, +Z.
    constexpr f32 kLen = 1.5f;
    const auto push = [&data](const math::Vector3& from, const math::Vector3& to) {
        const u32 base = static_cast<u32>(data->vertices.size());
        data->vertices.push_back({ from, math::Vector3::zero(), {} });
        data->vertices.push_back({ to, math::Vector3::zero(), {} });
        data->indices.push_back(base);
        data->indices.push_back(base + 1);
    };
    push(math::Vector3::zero(), math::Vector3(kLen, 0.0f, 0.0f));
    push(math::Vector3::zero(), math::Vector3(0.0f, kLen, 0.0f));
    push(math::Vector3::zero(), math::Vector3(0.0f, 0.0f, kLen));
    return data;
}

MeshDataPtr OpenGLRenderer::buildGrid() {
    auto data = std::make_shared<MeshData>();
    constexpr f32 kExtent = 12.0f;
    constexpr int kCells = 24;
    const f32 step = (kExtent * 2.0f) / static_cast<f32>(kCells);
    for (int i = 0; i <= kCells; ++i) {
        const f32 coord = -kExtent + step * static_cast<f32>(i);
        const u32 base = static_cast<u32>(data->vertices.size());
        data->vertices.push_back({ { coord, 0.0f, -kExtent }, math::Vector3::up(), {} });
        data->vertices.push_back({ { coord, 0.0f,  kExtent }, math::Vector3::up(), {} });
        data->vertices.push_back({ { -kExtent, 0.0f, coord }, math::Vector3::up(), {} });
        data->vertices.push_back({ {  kExtent, 0.0f, coord }, math::Vector3::up(), {} });
        data->indices.push_back(base);
        data->indices.push_back(base + 1);
        data->indices.push_back(base + 2);
        data->indices.push_back(base + 3);
    }
    return data;
}

void OpenGLRenderer::shutdown() {
    GlFunctions& g = gl();
    if (!g.GlDeleteFramebuffers) {
        return;
    }
    for (auto& [mesh, gpu] : meshCache_) {
        gpu.release();
    }
    meshCache_.clear();
    gridMesh_.release();
    gridData_.reset();
    axisMesh_.release();
    axisData_.reset();
    destroyViewportFramebuffer();
}

void OpenGLRenderer::setViewportSize(i32 width, i32 height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    if (width == vpW_ && height == vpH_) {
        return;
    }
    createViewportFramebuffer(width, height);
}

bool OpenGLRenderer::createViewportFramebuffer(i32 width, i32 height) {
    GlFunctions& g = gl();
    if (fbo_ == 0) {
        g.GlGenFramebuffers(1, &fbo_);
        g.GlGenTextures(1, &colorTex_);
        g.GlGenRenderbuffers(1, &depthRbo_);
    }

    g.GlBindTexture(GL_TEXTURE_2D, colorTex_);
    g.GlTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA8), width, height, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    g.GlTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    g.GlBindTexture(GL_TEXTURE_2D, 0);

    g.GlBindRenderbuffer(GL_RENDERBUFFER, depthRbo_);
    g.GlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    g.GlBindRenderbuffer(GL_RENDERBUFFER, 0);

    g.GlBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    g.GlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_, 0);
    g.GlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo_);

    const GLenum status = g.GlCheckFramebufferStatus(GL_FRAMEBUFFER);
    g.GlBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        core::Log::instance().error("El framebuffer del viewport está incompleto.");
        return false;
    }

    vpW_ = width;
    vpH_ = height;
    return true;
}

void OpenGLRenderer::destroyViewportFramebuffer() {
    GlFunctions& g = gl();
    if (fbo_ != 0) {
        g.GlDeleteFramebuffers(1, &fbo_);
        fbo_ = 0;
    }
    if (colorTex_ != 0) {
        g.GlDeleteTextures(1, &colorTex_);
        colorTex_ = 0;
    }
    if (depthRbo_ != 0) {
        g.GlDeleteRenderbuffers(1, &depthRbo_);
        depthRbo_ = 0;
    }
    vpW_ = vpH_ = 0;
}

void OpenGLRenderer::beginScene(const Camera& camera, const Lights& lights,
                                const math::Color& clearColor) {
    GlFunctions& g = gl();
    view_ = camera.viewMatrix();
    projection_ = camera.projectionMatrix(
        static_cast<f32>(vpW_) / static_cast<f32>(std::max(vpH_, 1)));
    cameraPosition_ = camera.position;
    lights_ = lights;

    if (vpW_ == 0 || vpH_ == 0) {
        setViewportSize(800, 600);
    }

    g.GlBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    g.GlViewport(0, 0, vpW_, vpH_);
    g.GlClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    g.GlClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::drawMeshObject(const scene::GameObject& object,
                                    const scene::MeshComponent& mesh) {
    const auto* data = mesh.data().get();
    if (!data || data->isEmpty()) {
        return;
    }

    GpuMesh& gpu = meshCacheFor(const_cast<scene::MeshComponent&>(mesh));
    const render::MaterialPtr material = mesh.material();
    const math::Matrix4 model = object.worldMatrix();

    if (wireframe_) {
        // Vista de alambres: contorno con el color del material (no iluminado).
        GlFunctions& gfx = gl();
        gfx.GlPolygonMode(GL_FRONT_AND_BACK, GL_LINE_MODE);
        unlitProgram_.use();
        unlitProgram_.setMat4("uModel", model);
        unlitProgram_.setMat4("uView", view_);
        unlitProgram_.setMat4("uProjection", projection_);
        const math::Color& c = material ? material->baseColor : math::Color::gray();
        unlitProgram_.setVec4("uBaseColor", c.r, c.g, c.b, 1.0f);
        gpu.draw();
        unlitProgram_.release();
        gfx.GlPolygonMode(GL_FRONT_AND_BACK, GL_FILL_MODE);
        return;
    }

    litProgram_.use();
    litProgram_.setMat4("uModel", model);
    litProgram_.setMat4("uView", view_);
    litProgram_.setMat4("uProjection", projection_);
    litProgram_.setMat3("uNormalMatrix", model.normalMatrix());

    const math::Color& c = material ? material->baseColor : math::Color::gray();
    litProgram_.setVec4("uBaseColor", c.r, c.g, c.b, c.a);
    litProgram_.setFloat("uShininess", material ? material->shininess : 48.0f);

    const bool hasTex = material && material->hasTexture();
    litProgram_.setInt("uHasTexture", hasTex ? 1 : 0);
    if (hasTex) {
        material->albedo->bind(0);
        litProgram_.setInt("uAlbedo", 0);
    }

    litProgram_.setVec3("uSunDirection", lights_.sunDirection.x, lights_.sunDirection.y,
                        lights_.sunDirection.z);
    litProgram_.setVec3("uSunColor", lights_.sunColor.x, lights_.sunColor.y, lights_.sunColor.z);
    litProgram_.setVec3("uAmbient", lights_.ambient.x, lights_.ambient.y, lights_.ambient.z);
    litProgram_.setVec3("uCameraPos", cameraPosition_.x, cameraPosition_.y, cameraPosition_.z);

    gpu.draw();
    litProgram_.release();
}

void OpenGLRenderer::drawGrid(const Camera& camera) {
    unlitProgram_.use();
    unlitProgram_.setMat4("uModel", math::Matrix4::identity());
    unlitProgram_.setMat4("uView", camera.viewMatrix());
    unlitProgram_.setMat4("uProjection",
                          camera.projectionMatrix(static_cast<f32>(vpW_) /
                                                  static_cast<f32>(std::max(vpH_, 1))));
    unlitProgram_.setVec4("uBaseColor", 0.16f, 0.22f, 0.34f, 0.55f);
    gridMesh_.draw(GL_LINES);
    unlitProgram_.release();
}

void OpenGLRenderer::drawAxes(const Camera& camera) {
    unlitProgram_.use();
    unlitProgram_.setMat4("uModel", math::Matrix4::identity());
    unlitProgram_.setMat4("uView", camera.viewMatrix());
    unlitProgram_.setMat4("uProjection",
                          camera.projectionMatrix(static_cast<f32>(vpW_) /
                                                  static_cast<f32>(std::max(vpH_, 1))));
    unlitProgram_.setVec4("uBaseColor", 0.55f, 0.68f, 0.95f, 0.9f);
    axisMesh_.draw(GL_LINES);
    unlitProgram_.release();
}

void OpenGLRenderer::endScene() {
    // Se mantiene el FBO enlazado para permitir la lectura de píxeles.
}

void OpenGLRenderer::presentToScreen() {
    GlFunctions& g = gl();
    g.GlBindFramebuffer(GL_FRAMEBUFFER, 0);
    g.GlViewport(0, 0, vpW_, vpH_);
}

void OpenGLRenderer::readViewportRGBA(std::vector<u8>& rgba) const {
    rgba.clear();
    if (vpW_ <= 0 || vpH_ <= 0) {
        return;
    }
    GlFunctions& g = gl();
    rgba.resize(static_cast<size_t>(vpW_) * vpH_ * 4u);
    if (g.GlReadPixels) {
        g.GlReadPixels(0, 0, vpW_, vpH_, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }
}

GpuMesh& OpenGLRenderer::meshCacheFor(scene::MeshComponent& mesh) {
    const auto existing = meshCache_.find(&mesh);
    if (existing == meshCache_.end()) {
        GpuMesh gpu;
        if (mesh.data()) {
            gpu.upload(*mesh.data());
        }
        auto [it, inserted] = meshCache_.emplace(&mesh, std::move(gpu));
        (void)inserted;
        return it->second;
    }
    // Re-sincroniza si los datos del vértice cambiaron (edición del polígono).
    const auto revIt = meshRevisions_.find(&mesh);
    const core::u64 lastRev = (revIt != meshRevisions_.end()) ? revIt->second : 0u;
    if (lastRev != mesh.revision()) {
        if (mesh.data()) {
            existing->second.upload(*mesh.data());
        }
        meshRevisions_[&mesh] = mesh.revision();
    }
    return existing->second;
}

} // namespace ue::render