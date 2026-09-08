#pragma once

#include "ue/scene/Component.hpp"
#include "ue/render/Material.hpp"
#include "ue/render/Vertex.hpp"

#include <string>

namespace ue::scene {

/// Componente que convierte a un GameObject en una malla renderizable.
class MeshComponent final : public Component {
public:
    explicit MeshComponent(render::MeshDataPtr data = nullptr,
                           render::MaterialPtr material = nullptr);

    void setData(render::MeshDataPtr data);
    render::MeshDataPtr data() const { return meshData_; }

    void setMaterial(render::MaterialPtr material) { material_ = std::move(material); }
    render::MaterialPtr material() const { return material_; }

    /// Nombre de la primitiva que originó la malla ("cubo", "esfera", ...).
    std::string primitiveType;

    /// Aumenta cada vez que cambian los datos (para re-subida eficiente a GPU).
    u64 revision() const { return revision_; }

    void markEdited() { ++revision_; }

private:
    render::MeshDataPtr meshData_;
    render::MaterialPtr material_;
    u64 revision_ = 0;
};

/// Crea un GameObject con una malla y material (Sugar para el editor).
std::shared_ptr<GameObject> createMeshObject(const std::string& name,
                                             render::MeshDataPtr data,
                                             render::MaterialPtr material);

} // namespace ue::scene