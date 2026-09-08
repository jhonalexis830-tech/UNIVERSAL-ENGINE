#pragma once

#include "ue/core/IService.hpp"
#include "ue/core/Types.hpp"
#include "ue/render/GpuMesh.hpp"
#include "ue/render/Material.hpp"

#include <vector>

namespace ue::platform {
class IWindow;
}

namespace ue::scene {
class Scene;
class GameObject;
class MeshComponent;
}

namespace ue::render {

class Camera;
struct Lights;

/// Contrato del renderizador. La implementación OpenGL (OpenGLRenderer) es
/// intercambiable: se puede sustituir por Vulkan, DirectX, o un rasterizador
/// por software sin tocar el resto del motor (DIP).
class IRenderer : public core::IService {
public:
    ~IRenderer() override = default;

    /// Prepara el contexto (carga funciones GL, compila shaders, crea el FBO).
    virtual bool initialize(platform::IWindow& window) = 0;
    virtual void shutdown() = 0;

    /// Redimensiona el framebuffer del viewport (en píxeles físicos).
    virtual void setViewportSize(i32 width, i32 height) = 0;

    /// Alterna el modo de dibujo de las mallas: true = alámbrico, false = sólido.
    virtual void setWireframe(bool enabled) = 0;

    /// Inicia el fotograma: enlaza el FBO, limpia y fija matrices de cámara/luces.
    virtual void beginScene(const Camera& camera, const Lights& lights,
                            const math::Color& clearColor) = 0;

    /// Dibuja un objeto con su malla y material (SRP: transporte de datos).
    virtual void drawMeshObject(const scene::GameObject& object,
                                const scene::MeshComponent& mesh) = 0;

    /// Dibuja la rejilla del suelo.
    virtual void drawGrid(const Camera& camera) = 0;

    /// Dibuja el marcador de ejes XYZ en el origen (líneas no iluminadas).
    virtual void drawAxes(const Camera& camera) = 0;

    /// Finaliza el fotograma del viewport (si es necesario).
    virtual void endScene() = 0;

    /// Vuelve al framebuffer de la pantalla (presenta la UI sobre el viewport).
    virtual void presentToScreen() = 0;

    /// Identificador GL de la textura de color del viewport.
    virtual u32 viewportColorTexture() const = 0;
    virtual i32 viewportWidth() const = 0;
    virtual i32 viewportHeight() const = 0;

    /// Lee los píxeles del viewport en RGBA8 (filas de abajo hacia arriba).
    virtual void readViewportRGBA(std::vector<u8>& rgba) const = 0;

    /// Garantiza que la malla está subida a la GPU y sincronizada con la CPU.
    /// Devuelve la instancia GPU para poder dibujarla.
    virtual GpuMesh& meshCacheFor(scene::MeshComponent& mesh) = 0;
};

} // namespace ue::render