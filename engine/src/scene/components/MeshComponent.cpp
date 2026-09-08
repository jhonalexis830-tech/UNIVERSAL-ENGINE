#include "ue/scene/components/MeshComponent.hpp"

#include "ue/scene/GameObject.hpp"

namespace ue::scene {

MeshComponent::MeshComponent(render::MeshDataPtr data, render::MaterialPtr material)
    : meshData_(std::move(data)), material_(material ? std::move(material)
                                                     : std::make_shared<render::Material>()) {
    if (meshData_) {
        ++revision_;
    }
}

void MeshComponent::setData(render::MeshDataPtr data) {
    meshData_ = std::move(data);
    ++revision_;
}

std::shared_ptr<GameObject> createMeshObject(const std::string& name,
                                             render::MeshDataPtr data,
                                             render::MaterialPtr material) {
    auto object = std::make_shared<GameObject>(name);
    auto mesh = object->addComponent<MeshComponent>(data, material);
    mesh->setData(data);
    if (material) {
        mesh->setMaterial(material);
    }
    return object;
}

} // namespace ue::scene