#pragma once

#include "ue/core/Types.hpp"

namespace ue::core {

/// Marcador de interfaz para todo servicio registrable.
/// (Principio de inversión de dependencias: las dependencias apuntan a interfaces).
class IService {
public:
    virtual ~IService() = default;
};

} // namespace ue::core