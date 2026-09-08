#include "ue/render/ShaderProgram.hpp"

#include "ue/core/Log.hpp"
#include "ue/math/Matrix4.hpp"

#include <vector>

namespace ue::render {

namespace gl = ue::render;
using ue::core::Log;

ShaderProgram::~ShaderProgram() {
    if (program_ != 0 && GlLoader::isValid()) {
        GlFunctions& g = GlLoader::functions();
        if (g.GlDeleteProgram) {
            g.GlDeleteProgram(program_);
        }
    }
    program_ = 0;
}

u32 ShaderProgram::compileShader(u32 type, const std::string& source, bool& ok) {
    GlFunctions& g = GlLoader::functions();
    const u32 shader = g.GlCreateShader(type);
    const char* src = source.c_str();
    g.GlShaderSource(shader, 1, &src, nullptr);
    g.GlCompileShader(shader);

    GLint compiled = 0;
    g.GlGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        logShaderError(shader, type);
        g.GlDeleteShader(shader);
        ok = false;
        return 0;
    }
    ok = true;
    return shader;
}

void ShaderProgram::logShaderError(u32 shader, u32 type) {
    GlFunctions& g = GlLoader::functions();
    GLint length = 0;
    g.GlGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> buffer(static_cast<size_t>(length > 0 ? length : 1));
    if (length > 0) {
        g.GlGetShaderInfoLog(shader, length, nullptr, buffer.data());
    }
    const char* kind = (type == GL_VERTEX_SHADER) ? "vértice" : "fragmento";
    Log::instance().errorFmt("Error al compilar el shader de {}: {}", kind, buffer.data());
}

void ShaderProgram::logProgramError(u32 program) {
    GlFunctions& g = GlLoader::functions();
    GLint length = 0;
    g.GlGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> buffer(static_cast<size_t>(length > 0 ? length : 1));
    if (length > 0) {
        g.GlGetProgramInfoLog(program, length, nullptr, buffer.data());
    }
    Log::instance().errorFmt("Error al enlazar el programa: {}", buffer.data());
}

bool ShaderProgram::build(const std::string& vertexSource, const std::string& fragmentSource) {
    GlFunctions& g = GlLoader::functions();
    if (!g.GlCreateShader || !g.GlCreateProgram) {
        return false;
    }

    bool okVertex = false;
    const u32 vertex = compileShader(GL_VERTEX_SHADER, vertexSource, okVertex);
    if (!okVertex) {
        return false;
    }

    bool okFragment = false;
    const u32 fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource, okFragment);
    if (!okFragment) {
        g.GlDeleteShader(vertex);
        return false;
    }

    program_ = g.GlCreateProgram();
    g.GlAttachShader(program_, vertex);
    g.GlAttachShader(program_, fragment);
    g.GlLinkProgram(program_);

    GLint linked = 0;
    g.GlGetProgramiv(program_, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        logProgramError(program_);
        g.GlDeleteProgram(program_);
        program_ = 0;
    }

    g.GlDeleteShader(vertex);
    g.GlDeleteShader(fragment);
    return program_ != 0;
}

void ShaderProgram::use() const {
    GlLoader::functions().GlUseProgram(program_);
}

void ShaderProgram::release() const {
    GlLoader::functions().GlUseProgram(0);
}

GLint ShaderProgram::uniformLocation(const std::string& name) const {
    const auto it = uniformCache_.find(name);
    if (it != uniformCache_.end()) {
        return it->second;
    }
    const GLint loc = GlLoader::functions().GlGetUniformLocation(program_, name.c_str());
    uniformCache_[name] = loc;
    return loc;
}

void ShaderProgram::setMat4(const std::string& name, const math::Matrix4& matrix) {
    GlLoader::functions().GlUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, matrix.m);
}

void ShaderProgram::setMat3(const std::string& name, const math::Matrix4& matrix) {
    // Submatriz 3x3 extraída del parámetro (columnas 0..2).
    const float values[9] = {
        matrix.m[0], matrix.m[1], matrix.m[2],
        matrix.m[4], matrix.m[5], matrix.m[6],
        matrix.m[8], matrix.m[9], matrix.m[10],
    };
    GlLoader::functions().GlUniformMatrix3fv(uniformLocation(name), 1, GL_FALSE, values);
}

void ShaderProgram::setVec3(const std::string& name, f32 x, f32 y, f32 z) {
    const float v[3] = { x, y, z };
    GlLoader::functions().GlUniform3fv(uniformLocation(name), 1, v);
}

void ShaderProgram::setVec4(const std::string& name, f32 r, f32 g, f32 b, f32 a) {
    const float v[4] = { r, g, b, a };
    GlLoader::functions().GlUniform4fv(uniformLocation(name), 1, v);
}

void ShaderProgram::setFloat(const std::string& name, f32 value) {
    GlLoader::functions().GlUniform1f(uniformLocation(name), value);
}

void ShaderProgram::setInt(const std::string& name, i32 value) {
    GlLoader::functions().GlUniform1i(uniformLocation(name), value);
}

} // namespace ue::render