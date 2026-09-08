#pragma once

#include "ue/core/Types.hpp"

#include <string>

namespace ue::core {

/// Identificador único de 64 bits para objetos, clips, escenas, etc.
class Uuid {
public:
    Uuid() : value_(generate()) {}
    Uuid(u64 value) : value_(value) {}
    explicit Uuid(const std::string& hex);

    u64 value() const { return value_; }
    std::string toHex() const;

    bool operator==(const Uuid& other) const { return value_ == other.value_; }
    bool operator!=(const Uuid& other) const { return value_ != other.value_; }
    bool operator<(const Uuid& other) const { return value_ < other.value_; }

    static const Uuid none() { return Uuid(0ULL); }

private:
    static u64 generate();
    u64 value_;
};

} // namespace ue::core