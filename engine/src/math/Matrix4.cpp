#include "ue/math/Matrix4.hpp"

#include <cmath>

namespace ue::math {

Matrix4 Matrix4::identity() {
    Matrix4 r;
    return r;
}

Matrix4 Matrix4::translation(f32 x, f32 y, f32 z) {
    Matrix4 r;
    r(3, 0) = x;
    r(3, 1) = y;
    r(3, 2) = z;
    return r;
}

Matrix4 Matrix4::rotationX(f32 degrees) {
    const f32 a = degToRad(degrees);
    const f32 c = std::cos(a), s = std::sin(a);
    Matrix4 r;
    r(1, 1) = c;  r(2, 1) = -s;
    r(1, 2) = s;  r(2, 2) = c;
    return r;
}

Matrix4 Matrix4::rotationY(f32 degrees) {
    const f32 a = degToRad(degrees);
    const f32 c = std::cos(a), s = std::sin(a);
    Matrix4 r;
    r(0, 0) = c;  r(2, 0) = s;
    r(0, 2) = -s; r(2, 2) = c;
    return r;
}

Matrix4 Matrix4::rotationZ(f32 degrees) {
    const f32 a = degToRad(degrees);
    const f32 c = std::cos(a), s = std::sin(a);
    Matrix4 r;
    r(0, 0) = c;  r(1, 0) = -s;
    r(0, 1) = s;  r(1, 1) = c;
    return r;
}

Matrix4 Matrix4::rotationEuler(f32 degX, f32 degY, f32 degZ) {
    // Orden de aplicación: primero Z (roll), luego Y (yaw), luego X (pitch).
    return rotationX(degX) * rotationY(degY) * rotationZ(degZ);
}

Matrix4 Matrix4::scale(f32 x, f32 y, f32 z) {
    Matrix4 r;
    r(0, 0) = x;
    r(1, 1) = y;
    r(2, 2) = z;
    return r;
}

Matrix4 Matrix4::compose(const Vector3& pos, const Vector3& rotDeg, const Vector3& scl) {
    const Matrix4 r = rotationEuler(rotDeg.x, rotDeg.y, rotDeg.z);
    const Matrix4 t = translation(pos);
    return t * r * scale(scl);
}

Matrix4 Matrix4::perspective(f32 fovYDeg, f32 aspect, f32 nearPlane, f32 farPlane) {
    const f32 f = 1.0f / std::tan(degToRad(fovYDeg) * 0.5f);
    const f32 nf = 1.0f / (nearPlane - farPlane);
    Matrix4 r;
    r(0, 0) = f / aspect;
    r(1, 1) = f;
    r(2, 2) = (farPlane + nearPlane) * nf;
    r(2, 3) = -1.0f;
    r(3, 2) = (2.0f * farPlane * nearPlane) * nf;
    r(3, 3) = 0.0f;
    return r;
}

Matrix4 Matrix4::lookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    const Vector3 fwd = (center - eye).normalized();
    const Vector3 side = fwd.cross(up).normalized();
    const Vector3 upv = side.cross(fwd);

    Matrix4 r;
    r(0, 0) = side.x;  r(1, 0) = side.y;  r(2, 0) = side.z;
    r(0, 1) = upv.x;   r(1, 1) = upv.y;   r(2, 1) = upv.z;
    r(0, 2) = -fwd.x;  r(1, 2) = -fwd.y;  r(2, 2) = -fwd.z;
    r(3, 0) = -side.dot(eye);
    r(3, 1) = -upv.dot(eye);
    r(3, 2) = fwd.dot(eye);
    return r;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 r;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            f32 sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += this->m[k * 4 + row] * other.m[col * 4 + k];
            }
            r(col, row) = sum;
        }
    }
    return r;
}

Vector3 Matrix4::transformPoint(const Vector3& p) const {
    const f32 w = m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15];
    const f32 invW = (w != 0.0f) ? 1.0f / w : 1.0f;
    return {
        (m[0] * p.x + m[4] * p.y + m[8]  * p.z + m[12]) * invW,
        (m[1] * p.x + m[5] * p.y + m[9]  * p.z + m[13]) * invW,
        (m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14]) * invW,
    };
}

Vector3 Matrix4::transformVector(const Vector3& v) const {
    return {
        m[0] * v.x + m[4] * v.y + m[8]  * v.z,
        m[1] * v.x + m[5] * v.y + m[9]  * v.z,
        m[2] * v.x + m[6] * v.y + m[10] * v.z,
    };
}

Matrix4 Matrix4::transposed() const {
    Matrix4 r;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            r(row, col) = this->m[col * 4 + row];
        }
    }
    return r;
}

Matrix4 Matrix4::inverse() const {
    return inverseGeneral(*this);
}

Matrix4 Matrix4::inverseGeneral(const Matrix4& mat) {
    // Algoritmo clásico por menores/cofactores (mano derecha, columna-mayor).
    const f32* m = mat.m;
    float inv[16];

    inv[0]  =  m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
             + m[9]*m[7]*m[14]  + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
    inv[4]  = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
             - m[8]*m[7]*m[14]  - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
    inv[8]  =  m[4]*m[9]*m[15]  - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
             + m[8]*m[7]*m[13]  + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
    inv[12] = -m[4]*m[9]*m[14]  + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
             - m[8]*m[6]*m[13]  - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];

    inv[1]  = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
             - m[9]*m[3]*m[14]  - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
    inv[5]  =  m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
             + m[8]*m[3]*m[14]  + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
    inv[9]  = -m[0]*m[9]*m[15]  + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
             - m[8]*m[3]*m[13]  - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
    inv[13] =  m[0]*m[9]*m[14]  - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
             + m[8]*m[2]*m[13]  + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];

    inv[2]  =  m[1]*m[6]*m[15]  - m[1]*m[7]*m[14]  - m[5]*m[2]*m[15]
             + m[5]*m[3]*m[14]  + m[13]*m[2]*m[7]  - m[13]*m[3]*m[6];
    inv[6]  = -m[0]*m[6]*m[15]  + m[0]*m[7]*m[14]  + m[4]*m[2]*m[15]
             - m[4]*m[3]*m[14]  - m[12]*m[2]*m[7]  + m[12]*m[3]*m[6];
    inv[10] =  m[0]*m[5]*m[15]  - m[0]*m[7]*m[13]  - m[4]*m[1]*m[15]
             + m[4]*m[3]*m[13]  + m[12]*m[1]*m[7]  - m[12]*m[3]*m[5];
    inv[14] = -m[0]*m[5]*m[14]  + m[0]*m[6]*m[13]  + m[4]*m[1]*m[14]
             - m[4]*m[2]*m[13]  - m[12]*m[1]*m[6]  + m[12]*m[2]*m[5];

    inv[3]  = -m[1]*m[6]*m[11]  + m[1]*m[7]*m[10]  + m[5]*m[2]*m[11]
             - m[5]*m[3]*m[10]  - m[9]*m[2]*m[7]   + m[9]*m[3]*m[6];
    inv[7]  =  m[0]*m[6]*m[11]  - m[0]*m[7]*m[10]  - m[4]*m[2]*m[11]
             + m[4]*m[3]*m[10]  + m[8]*m[2]*m[7]   - m[8]*m[3]*m[6];
    inv[11] = -m[0]*m[5]*m[11]  + m[0]*m[7]*m[9]   + m[4]*m[1]*m[11]
             - m[4]*m[3]*m[9]   - m[8]*m[1]*m[7]   + m[8]*m[3]*m[5];
    inv[15] =  m[0]*m[5]*m[10]  - m[0]*m[6]*m[9]   - m[4]*m[1]*m[10]
             + m[4]*m[2]*m[9]   + m[8]*m[1]*m[6]   - m[8]*m[2]*m[5];

    const f32 det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
    if (det == 0.0f) {
        return identity(); // matriz singular: no invertible, se devuelve identidad
    }
    const f32 invDet = 1.0f / det;
    Matrix4 r;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            r(col, row) = inv[col * 4 + row] * invDet;
        }
    }
    return r;
}

Matrix4 Matrix4::normalMatrix() const {
    // Extrae la submatriz 3x3 (columnas 0..2).
    Matrix4 sub;
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            sub(col, row) = (*this)(col, row);
        }
    }
    sub(3, 3) = 1.0f;
    Matrix4 invTrans = sub.inverse().transposed();
    Matrix4 r;
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            r(col, row) = invTrans(col, row);
        }
    }
    r(3, 3) = 1.0f;
    return r;
}

} // namespace ue::math