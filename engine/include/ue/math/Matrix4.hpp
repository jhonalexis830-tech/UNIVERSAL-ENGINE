#pragma once

#include "ue/core/Types.hpp"
#include "ue/math/Math.hpp"
#include "ue/math/Vector3.hpp"

namespace ue::math {

class Quaternion;

/// Matriz 4x4 de coma flotante, orden por columnas (estilo OpenGL).
/// El acceso es m[col * 4 + row].
class Matrix4 {
public:
    f32 m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    Matrix4() = default;

    /// Acceso col,row.
    f32& operator()(int col, int row) { return m[col * 4 + row]; }
    f32  operator()(int col, int row) const { return m[col * 4 + row]; }

    static Matrix4 identity();
    static Matrix4 translation(f32 x, f32 y, f32 z);
    static Matrix4 translation(const Vector3& t) { return translation(t.x, t.y, t.z); }
    static Matrix4 rotationX(f32 degrees);
    static Matrix4 rotationY(f32 degrees);
    static Matrix4 rotationZ(f32 degrees);
    static Matrix4 rotationEuler(f32 degX, f32 degY, f32 degZ);
    static Matrix4 scale(f32 x, f32 y, f32 z);
    static Matrix4 scale(const Vector3& s) { return scale(s.x, s.y, s.z); }

    /// Matriz TRS = Traslación * Rotación * Escala.
    static Matrix4 compose(const Vector3& pos, const Vector3& rotDeg, const Vector3& scale);

    /// Proyección en perspectiva (mano derecha, rango de profundidad 0..1).
    static Matrix4 perspective(f32 fovYDeg, f32 aspect, f32 nearPlane, f32 farPlane);

    /// Vista: matriz lookAt de mano derecha.
    static Matrix4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

    /// Aplica "other" y después "this": resultado = this * other.
    Matrix4 operator*(const Matrix4& other) const;

    Vector3 transformPoint(const Vector3& p) const;
    Vector3 transformVector(const Vector3& v) const;

    Matrix4 transposed() const;
    Matrix4 inverse() const;

    /// Matriz de normales (inversa-transpuesta de la submatriz 3x3).
    Matrix4 normalMatrix() const;

private:
    static Matrix4 inverseGeneral(const Matrix4& mat);
};

} // namespace ue::math