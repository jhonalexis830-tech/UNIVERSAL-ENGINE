#include "ue/math/Quaternion.hpp"

#include <cmath>

namespace ue::math {

Quaternion Quaternion::fromAxisAngle(const Vector3& axis, f32 degrees) {
    const Vector3 a = axis.normalized();
    const f32 half = degToRad(degrees) * 0.5f;
    const f32 s = std::sin(half);
    return Quaternion(std::cos(half), a.x * s, a.y * s, a.z * s).normalized();
}

Quaternion Quaternion::fromEulerDeg(f32 degX, f32 degY, f32 degZ) {
    const Quaternion qx = fromAxisAngle(Vector3::right(), degX);
    const Quaternion qy = fromAxisAngle(Vector3::up(), degY);
    const Quaternion qz = fromAxisAngle(Vector3::forward(), degZ);
    return (qx * qy * qz).normalized();
}

Quaternion Quaternion::normalized() const {
    const f32 l = std::sqrt(w * w + x * x + y * y + z * z);
    if (l < 1e-6f) {
        return identity();
    }
    const f32 inv = 1.0f / l;
    return Quaternion(w * inv, x * inv, y * inv, z * inv);
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::operator*(const Quaternion& o) const {
    return Quaternion(
        w * o.w - x * o.x - y * o.y - z * o.z,
        w * o.x + x * o.w + y * o.z - z * o.y,
        w * o.y - x * o.z + y * o.w + z * o.x,
        w * o.z + x * o.y - y * o.x + z * o.w
    );
}

Vector3 Quaternion::rotate(const Vector3& v) const {
    const Quaternion p(0.0f, v.x, v.y, v.z);
    const Quaternion res = (*this) * p * conjugate();
    return Vector3(res.x, res.y, res.z);
}

Quaternion Quaternion::nlerp(const Quaternion& a, const Quaternion& b, f32 t) {
    f32 bx = b.w, by = b.x, bz = b.y, bw = b.z;
    const f32 dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    if (dot < 0.0f) {
        bx = -b.w; by = -b.x; bz = -b.y; bw = -b.z;
    }
    const f32 oneT = 1.0f - t;
    Quaternion r(
        a.w * oneT + bx * t,
        a.x * oneT + by * t,
        a.y * oneT + bz * t,
        a.z * oneT + bw * t
    );
    return r.normalized();
}

} // namespace ue::math