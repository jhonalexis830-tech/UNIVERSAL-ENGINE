#pragma once

#include "ue/core/Types.hpp"
#include "ue/render/Gl.hpp"

#include <string>
#include <unordered_map>

namespace ue::math {
class Matrix4;
}

namespace ue::render {

/// Programa de shaders OpenGL (SRP: compilar/enlazar/uniforms en una clase).
class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    /// Compila vertex+fragment y enlaza; false si algo falla.
    bool build(const std::string& vertexSource, const std::string& fragmentSource);

    void use() const;
    void release() const;

    u32 programId() const { return program_; }
    bool isValid() const { return program_ != 0; }

    GLint uniformLocation(const std::string& name) const;

    void setMat4(const std::string& name, const math::Matrix4& matrix);
    void setMat3(const std::string& name, const math::Matrix4& matrix);
    void setVec3(const std::string& name, f32 x, f32 y, f32 z);
    void setVec4(const std::string& name, f32 r, f32 g, f32 b, f32 a);
    void setFloat(const std::string& name, f32 value);
    void setInt(const std::string& name, i32 value);

private:
    static u32 compileShader(u32 type, const std::string& source, bool& ok);
    static void logShaderError(u32 shader, u32 type);
    static void logProgramError(u32 program);

    u32 program_ = 0;
    mutable std::unordered_map<std::string, GLint> uniformCache_;
};

} // namespace ue::render