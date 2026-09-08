#pragma once

#include "ue/core/IService.hpp"

namespace ue::core {

/// Contrato mínimo para cualquier módulo del motor que vive durante toda la
/// ejecución (render, física, animación, grabación, UI...).
class IEngineModule : public IService {
public:
    ~IEngineModule() override = default;

    virtual const char* moduleName() const = 0;
    virtual bool initialize() = 0;
    virtual void update(f64 deltaSeconds) = 0;
    virtual void shutdown() = 0;
};

} // namespace ue::core