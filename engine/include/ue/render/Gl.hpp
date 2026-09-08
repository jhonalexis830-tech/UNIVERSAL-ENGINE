#pragma once

#include "ue/core/Types.hpp"

#include <cstddef>

// ---------------------------------------------------------------
// Tipos y constantes de OpenGL mínimos usados por el motor.
// La carga de funciones se hace en GlLoader (sin GLAD/GLEW).
// ---------------------------------------------------------------

#if defined(_WIN32)
#define UE_GLAPIENTRY __stdcall
#else
#define UE_GLAPIENTRY
#endif

namespace ue::render {

using GLenum  = u32;
using GLuint  = u32;
using GLint   = i32;
using GLsizei = i32;
using GLfloat = f32;
using GLchar  = char;
using GLboolean = u8;
using GLvoid  = void;
using GLbitfield = u32;
using GLsizeiptr = std::ptrdiff_t;

// Constantes
constexpr GLboolean GL_TRUE  = 1;
constexpr GLboolean GL_FALSE = 0;

constexpr GLenum GL_VERSION             = 0x1F02;
constexpr GLenum GL_MAJOR_VERSION       = 0x821B;
constexpr GLenum GL_MINOR_VERSION       = 0x821C;
constexpr GLenum GL_VIEWPORT            = 0x0BA2;
constexpr GLenum GL_TRIANGLES           = 0x0004;
constexpr GLenum GL_LINES               = 0x0001;
constexpr GLenum GL_COLOR_BUFFER_BIT    = 0x00004000;
constexpr GLenum GL_DEPTH_BUFFER_BIT    = 0x00000100;
constexpr GLenum GL_DEPTH_TEST          = 0x0B71;
constexpr GLenum GL_LEQUAL              = 0x0203;
constexpr GLenum GL_DEPTH_WRITEMASK     = 0x0B72;
constexpr GLenum GL_BLEND               = 0x0BE2;
constexpr GLenum GL_SRC_ALPHA           = 0x0302;
constexpr GLenum GL_ONE_MINUS_SRC_ALPHA = 0x0303;
constexpr GLenum GL_CULL_FACE           = 0x0B44;
constexpr GLenum GL_BACK                = 0x0405;
constexpr GLenum GL_FRONT_AND_BACK      = 0x0408;
constexpr GLenum GL_POLYGON_MODE        = 0x0B40;
constexpr GLenum GL_LINE_MODE           = 0x1B01; // GL_LINE (modo poligonal)
constexpr GLenum GL_FILL_MODE           = 0x1B02; // GL_FILL
constexpr GLenum GL_FLOAT               = 0x1406;
constexpr GLenum GL_UNSIGNED_INT        = 0x1405;
constexpr GLenum GL_UNSIGNED_BYTE       = 0x1401;
constexpr GLenum GL_ARRAY_BUFFER        = 0x8892;
constexpr GLenum GL_ELEMENT_ARRAY_BUFFER= 0x8893;
constexpr GLenum GL_STATIC_DRAW         = 0x88E4;
constexpr GLenum GL_DYNAMIC_DRAW        = 0x88E8;
constexpr GLenum GL_VERTEX_SHADER       = 0x8B31;
constexpr GLenum GL_FRAGMENT_SHADER     = 0x8B32;
constexpr GLenum GL_COMPILE_STATUS      = 0x8B81;
constexpr GLenum GL_LINK_STATUS         = 0x8B82;
constexpr GLenum GL_INFO_LOG_LENGTH     = 0x8B84;
constexpr GLenum GL_TEXTURE_2D          = 0x0DE1;
constexpr GLenum GL_RGBA                = 0x1908;
constexpr GLenum GL_RGB                 = 0x1907;
constexpr GLenum GL_RGBA8               = 0x8058;
constexpr GLenum GL_TEXTURE_MIN_FILTER  = 0x2801;
constexpr GLenum GL_TEXTURE_MAG_FILTER  = 0x2800;
constexpr GLenum GL_TEXTURE_WRAP_S      = 0x2802;
constexpr GLenum GL_TEXTURE_WRAP_T      = 0x2803;
constexpr GLenum GL_LINEAR              = 0x2601;
constexpr GLenum GL_NEAREST             = 0x2600;
constexpr GLenum GL_REPEAT              = 0x2901;
constexpr GLenum GL_CLAMP_TO_EDGE       = 0x812F;
constexpr GLenum GL_TEXTURE0            = 0x84C0;
constexpr GLenum GL_FRAMEBUFFER         = 0x8D40;
constexpr GLenum GL_RENDERBUFFER        = 0x8D41;
constexpr GLenum GL_FRAMEBUFFER_COMPLETE= 0x8CD5;
constexpr GLenum GL_COLOR_ATTACHMENT0   = 0x8CE0;
constexpr GLenum GL_DEPTH_ATTACHMENT    = 0x8D00;
constexpr GLenum GL_DEPTH_COMPONENT24   = 0x81A6;
constexpr GLenum GL_MAX_SAMPLES         = 0x8D57;
constexpr GLenum GL_SCISSOR_TEST        = 0x0C11;

// Punteros a funciones OpenGL
using PfnGlVersion            = const GLubyte* (UE_GLAPIENTRY*)(GLenum);
using PfnGlGetIntegerv        = void (UE_GLAPIENTRY*)(GLenum, GLint*);
using PfnGlViewport           = void (UE_GLAPIENTRY*)(GLint, GLint, GLsizei, GLsizei);
using PfnGlClearColor         = void (UE_GLAPIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat);
using PfnGlClear              = void (UE_GLAPIENTRY*)(GLbitfield);
using PfnGlEnable             = void (UE_GLAPIENTRY*)(GLenum);
using PfnGlDisable            = void (UE_GLAPIENTRY*)(GLenum);
using PfnGlDepthFunc          = void (UE_GLAPIENTRY*)(GLenum);
using PfnGlDepthMask          = void (UE_GLAPIENTRY*)(GLboolean);
using PfnGlBlendFunc          = void (UE_GLAPIENTRY*)(GLenum, GLenum);
using PfnGlCullFace           = void (UE_GLAPIENTRY*)(GLenum);
using PfnGlPolygonMode        = void (UE_GLAPIENTRY*)(GLenum, GLenum);
using PfnGlCreateShader       = GLuint (UE_GLAPIENTRY*)(GLenum);
using PfnGlShaderSource       = void (UE_GLAPIENTRY*)(GLuint, GLsizei, const GLchar* const*, const GLint*);
using PfnGlCompileShader      = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlGetShaderiv        = void (UE_GLAPIENTRY*)(GLuint, GLenum, GLint*);
using PfnGlGetShaderInfoLog   = void (UE_GLAPIENTRY*)(GLuint, GLsizei, GLsizei*, GLchar*);
using PfnGlDeleteShader       = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlCreateProgram      = GLuint (UE_GLAPIENTRY*)();
using PfnGlAttachShader       = void (UE_GLAPIENTRY*)(GLuint, GLuint);
using PfnGlLinkProgram        = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlGetProgramiv       = void (UE_GLAPIENTRY*)(GLuint, GLenum, GLint*);
using PfnGlGetProgramInfoLog  = void (UE_GLAPIENTRY*)(GLuint, GLsizei, GLsizei*, GLchar*);
using PfnGlDeleteProgram      = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlUseProgram         = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlGenVertexArrays    = void (UE_GLAPIENTRY*)(GLsizei, GLuint*);
using PfnGlDeleteVertexArrays = void (UE_GLAPIENTRY*)(GLsizei, const GLuint*);
using PfnGlBindVertexArray    = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlGenBuffers         = void (UE_GLAPIENTRY*)(GLsizei, GLuint*);
using PfnGlDeleteBuffers      = void (UE_GLAPIENTRY*)(GLsizei, const GLuint*);
using PfnGlBindBuffer         = void (UE_GLAPIENTRY*)(GLenum, GLuint);
using PfnGlBufferData         = void (UE_GLAPIENTRY*)(GLenum, GLsizeiptr, const GLvoid*, GLenum);
using PfnGlVertexAttribPointer= void (UE_GLAPIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
using PfnGlEnableVertexAttribArray = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlDisableVertexAttribArray = void (UE_GLAPIENTRY*)(GLuint);
using PfnGlDrawArrays         = void (UE_GLAPIENTRY*)(GLenum, GLint, GLsizei);
using PfnGlDrawElements       = void (UE_GLAPIENTRY*)(GLenum, GLsizei, GLenum, const GLvoid*);
using PfnGlGetUniformLocation = GLint (UE_GLAPIENTRY*)(GLuint, const GLchar*);
using PfnGlUniformMatrix4fv   = void (UE_GLAPIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat*);
using PfnGlUniformMatrix3fv   = void (UE_GLAPIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat*);
using PfnGlUniform4fv         = void (UE_GLAPIENTRY*)(GLint, GLsizei, const GLfloat*);
using PfnGlUniform3fv         = void (UE_GLAPIENTRY*)(GLint, GLsizei, const GLfloat*);
using PfnGlUniform1i          = void (UE_GLAPIENTRY*)(GLint, GLint);
using PfnGlUniform1f          = void (UE_GLAPIENTRY*)(GLint, GLfloat);
using PfnGlGenTextures        = void (UE_GLAPIENTRY*)(GLsizei, GLuint*);
using PfnGlDeleteTextures     = void (UE_GLAPIENTRY*)(GLsizei, const GLuint*);
using PfnGlBindTexture        = void (UE_GLAPIENTRY*)(GLenum, GLuint);
using PfnGlTexImage2D         = void (UE_GLAPIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
using PfnGlTexParameteri      = void (UE_GLAPIENTRY*)(GLenum, GLenum, GLint);
using PfnGlActiveTexture      = void (UE_GLAPIENTRY*)(GLenum);
using PfnGlReadPixels         = void (UE_GLAPIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
using PfnGlGenFramebuffers    = void (UE_GLAPIENTRY*)(GLsizei, GLuint*);
using PfnGlBindFramebuffer    = void (UE_GLAPIENTRY*)(GLenum, GLuint);
using PfnGlFramebufferTexture2D = void (UE_GLAPIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint);
using PfnGlCheckFramebufferStatus = GLenum (UE_GLAPIENTRY*)(GLenum);
using PfnGlDeleteFramebuffers = void (UE_GLAPIENTRY*)(GLsizei, const GLuint*);
using PfnGlGenRenderbuffers   = void (UE_GLAPIENTRY*)(GLsizei, GLuint*);
using PfnGlBindRenderbuffer   = void (UE_GLAPIENTRY*)(GLenum, GLuint);
using PfnGlRenderbufferStorage = void (UE_GLAPIENTRY*)(GLenum, GLenum, GLsizei, GLsizei);
using PfnGlFramebufferRenderbuffer = void (UE_GLAPIENTRY*)(GLenum, GLenum, GLenum, GLuint);
using PfnGlDeleteRenderbuffers = void (UE_GLAPIENTRY*)(GLsizei, const GLuint*);
using PfnGlGetError           = GLenum (UE_GLAPIENTRY*)();
using PfnGlReadBuffer         = void (UE_GLAPIENTRY*)(GLenum);

struct GlFunctions {
    PfnGlVersion GlVersion = nullptr;
    PfnGlGetIntegerv GlGetIntegerv = nullptr;
    PfnGlViewport GlViewport = nullptr;
    PfnGlClearColor GlClearColor = nullptr;
    PfnGlClear GlClear = nullptr;
    PfnGlEnable GlEnable = nullptr;
    PfnGlDisable GlDisable = nullptr;
    PfnGlDepthFunc GlDepthFunc = nullptr;
    PfnGlDepthMask GlDepthMask = nullptr;
    PfnGlBlendFunc GlBlendFunc = nullptr;
    PfnGlCullFace GlCullFace = nullptr;
    PfnGlPolygonMode GlPolygonMode = nullptr;
    PfnGlCreateShader GlCreateShader = nullptr;
    PfnGlShaderSource GlShaderSource = nullptr;
    PfnGlCompileShader GlCompileShader = nullptr;
    PfnGlGetShaderiv GlGetShaderiv = nullptr;
    PfnGlGetShaderInfoLog GlGetShaderInfoLog = nullptr;
    PfnGlDeleteShader GlDeleteShader = nullptr;
    PfnGlCreateProgram GlCreateProgram = nullptr;
    PfnGlAttachShader GlAttachShader = nullptr;
    PfnGlLinkProgram GlLinkProgram = nullptr;
    PfnGlGetProgramiv GlGetProgramiv = nullptr;
    PfnGlGetProgramInfoLog GlGetProgramInfoLog = nullptr;
    PfnGlDeleteProgram GlDeleteProgram = nullptr;
    PfnGlUseProgram GlUseProgram = nullptr;
    PfnGlGenVertexArrays GlGenVertexArrays = nullptr;
    PfnGlDeleteVertexArrays GlDeleteVertexArrays = nullptr;
    PfnGlBindVertexArray GlBindVertexArray = nullptr;
    PfnGlGenBuffers GlGenBuffers = nullptr;
    PfnGlDeleteBuffers GlDeleteBuffers = nullptr;
    PfnGlBindBuffer GlBindBuffer = nullptr;
    PfnGlBufferData GlBufferData = nullptr;
    PfnGlVertexAttribPointer GlVertexAttribPointer = nullptr;
    PfnGlEnableVertexAttribArray GlEnableVertexAttribArray = nullptr;
    PfnGlDisableVertexAttribArray GlDisableVertexAttribArray = nullptr;
    PfnGlDrawArrays GlDrawArrays = nullptr;
    PfnGlDrawElements GlDrawElements = nullptr;
    PfnGlGetUniformLocation GlGetUniformLocation = nullptr;
    PfnGlUniformMatrix4fv GlUniformMatrix4fv = nullptr;
    PfnGlUniformMatrix3fv GlUniformMatrix3fv = nullptr;
    PfnGlUniform4fv GlUniform4fv = nullptr;
    PfnGlUniform3fv GlUniform3fv = nullptr;
    PfnGlUniform1i GlUniform1i = nullptr;
    PfnGlUniform1f GlUniform1f = nullptr;
    PfnGlGenTextures GlGenTextures = nullptr;
    PfnGlDeleteTextures GlDeleteTextures = nullptr;
    PfnGlBindTexture GlBindTexture = nullptr;
    PfnGlTexImage2D GlTexImage2D = nullptr;
    PfnGlTexParameteri GlTexParameteri = nullptr;
    PfnGlActiveTexture GlActiveTexture = nullptr;
    PfnGlReadPixels GlReadPixels = nullptr;
    PfnGlGenFramebuffers GlGenFramebuffers = nullptr;
    PfnGlBindFramebuffer GlBindFramebuffer = nullptr;
    PfnGlFramebufferTexture2D GlFramebufferTexture2D = nullptr;
    PfnGlCheckFramebufferStatus GlCheckFramebufferStatus = nullptr;
    PfnGlDeleteFramebuffers GlDeleteFramebuffers = nullptr;
    PfnGlGenRenderbuffers GlGenRenderbuffers = nullptr;
    PfnGlBindRenderbuffer GlBindRenderbuffer = nullptr;
    PfnGlRenderbufferStorage GlRenderbufferStorage = nullptr;
    PfnGlFramebufferRenderbuffer GlFramebufferRenderbuffer = nullptr;
    PfnGlDeleteRenderbuffers GlDeleteRenderbuffers = nullptr;
    PfnGlGetError GlGetError = nullptr;
};

/// Cargador de funciones OpenGL. Necesita una función de resolución de
/// procedimientos (p. ej. glfwGetProcAddress) tras crear el contexto.
class GlLoader {
public:
    using ProcLoader = void* (*)(const char*);

    static GlFunctions& functions();
    static bool load(ProcLoader loader);
    static bool isValid() { return loaded_; }

private:
    template <typename T>
    static void resolveProc(GlFunctions& gl, T& target, const char* name, ProcLoader loader);
    static bool loaded_;
};

} // namespace ue::render