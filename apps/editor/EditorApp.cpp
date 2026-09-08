#include "EditorApp.hpp"

#include "ue/animation/Animator.hpp"
#include "ue/core/Log.hpp"
#include "ue/core/SystemPaths.hpp"
#include "ue/editor/EditorTheme.hpp"
#include "ue/editor/IEditorUserInterface.hpp"
#include "ue/editor/UiSystem.hpp"
#include "ue/math/Color.hpp"
#include "ue/media/FrameRecorder.hpp"
#include "ue/physics/Physics.hpp"
#include "ue/platform/Input.hpp"
#include "ue/platform/Window.hpp"
#include "ue/render/Material.hpp"
#include "ue/render/OpenGLRenderer.hpp"
#include "ue/scene/MeshFactory.hpp"
#include "ue/scene/Scene.hpp"
#include "ue/scene/SceneSerializer.hpp"
#include "ue/scene/components/MeshComponent.hpp"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstring>
#include <filesystem>

using namespace ue;

namespace {
    // Azul metálico profundo del tema UNIVERSAL ENGINE.
    const ue::math::Color kClearColor(0.043f, 0.051f, 0.075f, 1.0f);
}

bool EditorApp::initialize(int argc, char* argv[]) {
    core::SystemPaths::ensureUserFolders();

    window_ = std::make_unique<platform::Window>();
    if (!window_->create(1600, 900, "UNIVERSAL ENGINE — TEAM S.H.O.T. WARE")) {
        core::Log::instance().error("No se pudo crear la ventana.");
        return false;
    }
    window_->setVsync(true);

    renderer_ = std::make_unique<render::OpenGLRenderer>();
    if (!renderer_->initialize(*window_)) {
        core::Log::instance().error("Fallo al inicializar el renderizador OpenGL.");
        return false;
    }

    // IMPORTANTE: Input debe crearse ANTES de inicializar ImGui (encadena los
    // callbacks de GLFW; ImGui se instala después y comparte los eventos).
    input_ = std::make_unique<platform::Input>(*window_);

    physics_ = std::make_unique<physics::PhysicsWorld>();
    recorder_ = std::make_unique<media::FrameRecorder>();
    animator_ = std::make_unique<anim::Animator>();

    prepareScene(argc, argv);

    ui_ = std::make_unique<editor::UiSystem>(*scene_, *renderer_, *input_, *physics_,
                                             *recorder_, *animator_);

    if (!initImGui()) {
        core::Log::instance().error("Fallo al inicializar la interfaz.");
        return false;
    }

    lastNowSeconds_ = glfwGetTime();
    core::Log::instance().info("Editor iniciado correctamente.");
    return true;
}

bool EditorApp::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    editor::EditorTheme::applyStyle();
    editor::EditorTheme::loadDefaultFont();

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window_->nativeHandle()), true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
    imguiReady_ = true;
    return true;
}

void EditorApp::prepareScene(int argc, char* argv[]) {
    std::string path;
    if (argc > 1) {
        path = argv[1];
    }

    if (!path.empty() && std::filesystem::exists(path)) {
        scene_ = std::make_shared<scene::Scene>("Escena cargada");
        if (scene::SceneSerializer::loadFromFile(*scene_, path)) {
            core::Log::instance().info("Escena cargada: " + path);
            return;
        }
        core::Log::instance().warn("No se pudo cargar la escena '" + path + "'; se usará una nueva.");
    }

    scene_ = scene::Scene::createTemplateScene();
}

void EditorApp::captureViewportIfRecording() {
    if (!ui_ || !ui_->state().recording) {
        return;
    }
    std::vector<u8> rgba;
    renderer_->readViewportRGBA(rgba);
    if (rgba.empty()) {
        return;
    }
    const i32 w = renderer_->viewportWidth();
    const i32 h = renderer_->viewportHeight();

    // readViewportRGBA devuelve las filas de abajo hacia arriba; la grabadora y
    // el BMP esperan la primera fila arriba. Se voltean las filas.
    std::vector<u8> flipped(rgba.size());
    const std::size_t rowBytes = static_cast<std::size_t>(w) * 4u;
    for (i32 y = 0; y < h; ++y) {
        std::memcpy(flipped.data() + static_cast<std::size_t>(y) * rowBytes,
                    rgba.data() + static_cast<std::size_t>(h - 1 - y) * rowBytes, rowBytes);
    }
    recorder_->captureFrame(flipped, w, h);
}

void EditorApp::run() {
    if (!window_ || !ui_) {
        return;
    }

    while (!window_->shouldClose() && !ui_->state().exitRequested) {
        const double now = glfwGetTime();
        time_.update(now);
        const float dt = static_cast<float>(time_.deltaSeconds());
        lastNowSeconds_ = now;

        window_->pollEvents();
        input_->beginFrame(window_->width(), window_->height());

        // Mantener el FBO del viewport del tamaño del framebuffer de la ventana.
        if (renderer_->viewportWidth() != window_->framebufferWidth() ||
            renderer_->viewportHeight() != window_->framebufferHeight()) {
            renderer_->setViewportSize(window_->framebufferWidth(), window_->framebufferHeight());
        }

        // Simulación.
        physics_->update(dt, *scene_);
        animator_->update(dt, *scene_);

        // Render del viewport (a la textura del FBO).
        renderer_->setWireframe(!ui_->state().shaded);
        renderer_->beginScene(scene_->camera, scene_->globalLights, kClearColor);
        if (ui_->state().showGrid) {
            renderer_->drawGrid(scene_->camera);
        }
        if (ui_->state().showAxis) {
            renderer_->drawAxes(scene_->camera);
        }
        for (scene::GameObject* object : scene_->allObjects()) {
            if (!object->visible) {
                continue;
            }
            const auto mesh = object->getComponent<scene::MeshComponent>();
            if (mesh && mesh->data() && !mesh->data()->isEmpty()) {
                renderer_->drawMeshObject(*object, *mesh);
            }
        }
        renderer_->endScene();

        captureViewportIfRecording();
        renderer_->presentToScreen();

        // Interfaz del editor sobre la pantalla.
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ui_->update(dt);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window_->swapBuffers();
    }
}

void EditorApp::shutdown() {
    // Cerrar la grabación pendiente antes de destruir el FBO.
    if (ui_ && ui_->state().recording) {
        recorder_->endSession(60.0);
    }

    if (imguiReady_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    if (renderer_) {
        renderer_->shutdown();
    }
    if (window_) {
        window_->destroy();
    }
    core::Log::instance().info("Editor cerrado.");
}