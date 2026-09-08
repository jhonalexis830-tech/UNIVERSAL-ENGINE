#include "ue/core/Uuid.hpp"

#include <random>

namespace ue::core {

Uuid::Uuid(const std::string& hex) : value_(0ULL) {
    u64 out = 0;
    u32 nibbles = 0;
    for (const char c : hex) {
        u8 n = 0;
        if (c >= '0' && c <= '9')      n = static_cast<u8>(c - '0');
        else if (c >= 'a' && c <= 'f') n = static_cast<u8>(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') n = static_cast<u8>(c - 'A' + 10);
        else continue;
        out = (out << 4) | n;
        ++nibbles;
    }
    if (nibbles > 16) {
        out = 0; // valor no representable en 64 bits
    }
    value_ = out;
}

std::string Uuid::toHex() const {
    static const char* digits = "0123456789abcdef";
    std::string result(16, '0');
    u64 v = value_;
    for (int i = 15; i >= 0; --i) {
        result[static_cast<size_t>(i)] = digits[v & 0xF];
        v >>= 4;
    }
    return result;
}

u64 Uuid::generate() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    u64 v = gen();
    if (v == 0ULL) {
        v = 1ULL; // 0 se reserva para "sin id"
    }
    return v;
}

} // namespace ue::core