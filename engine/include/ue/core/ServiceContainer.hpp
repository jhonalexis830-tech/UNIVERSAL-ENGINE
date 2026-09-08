#pragma once

#include "ue/core/IService.hpp"
#include "ue/core/Types.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <utility>

namespace ue::core {

/// Contenedor de servicios (Inyección de Dependencias).
/// Permite registrar una única instancia por tipo de interfaz y recuperarla
/// después sin acoplar los módulos entre sí (DIP).
class ServiceContainer {
public:
    ServiceContainer() = default;

    template <typename TInterface>
    bool has() const {
        static_assert(std::is_base_of_v<IService, TInterface>, "TInterface debe heredar de ue::core::IService");
        return services_.count(typeid(TInterface)) != 0;
    }

    template <typename TInterface, typename TImpl, typename... Args>
    TInterface& registerSingleton(Args&&... args) {
        static_assert(std::is_base_of_v<IService, TInterface>,
                      "TInterface debe heredar de ue::core::IService");
        static_assert(std::is_base_of_v<TInterface, TImpl>,
                      "TImpl debe implementar TInterface");
        auto instance = std::make_shared<TImpl>(std::forward<Args>(args)...);
        services_[typeid(TInterface)] = std::static_pointer_cast<IService>(instance);
        return static_cast<TInterface&>(*instance);
    }

    template <typename TInterface>
    void registerExternal(std::shared_ptr<TInterface> instance) {
        services_[typeid(TInterface)] = std::move(instance);
    }

    /// Devuelve la instancia registrada; falla en tiempo de ejecución si no existe.
    template <typename TInterface>
    TInterface& get() const {
        static_assert(std::is_base_of_v<IService, TInterface>,
                      "TInterface debe heredar de ue::core::IService");
        const auto it = services_.find(typeid(TInterface));
        assert(it != services_.end() && "Servicio no registrado en el contenedor");
        auto* raw = static_cast<TInterface*>(it->second.get());
        return *raw;
    }

private:
    std::map<std::type_index, std::shared_ptr<IService>> services_;
};

} // namespace ue::core