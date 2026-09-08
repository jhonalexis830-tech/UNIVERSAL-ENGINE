#include "ue/render/Gl.hpp"

namespace ue::render {

bool GlLoader::loaded_ = false;

GlFunctions& GlLoader::functions() {
    static GlFunctions gl;
    return gl;
}

#define UE_RESOLVE(member, pfnType, name) \
    do { \
        gl.member = reinterpret_cast<pfnType>(loader(name)); \
        if (gl.member == nullptr) return false; \
    } while (0)

bool GlLoader::load(ProcLoader loader) {
    if (!loader) {
        return false;
    }
    GlFunctions& gl = functions();

    UE_RESOLVE(GlVersion, PfnGlVersion, "glGetString");
    UE_RESOLVE(GlGetIntegerv, PfnGlGetIntegerv, "glGetIntegerv");
    UE_RESOLVE(GlViewport, PfnGlViewport, "glViewport");
    UE_RESOLVE(GlClearColor, PfnGlClearColor, "glClearColor");
    UE_RESOLVE(GlClear, PfnGlClear, "glClear");
    UE_RESOLVE(GlEnable, PfnGlEnable, "glEnable");
    UE_RESOLVE(GlDisable, PfnGlDisable, "glDisable");
    UE_RESOLVE(GlDepthFunc, PfnGlDepthFunc, "glDepthFunc");
    UE_RESOLVE(GlDepthMask, PfnGlDepthMask, "glDepthMask");
    UE_RESOLVE(GlBlendFunc, PfnGlBlendFunc, "glBlendFunc");
    UE_RESOLVE(GlCullFace, PfnGlCullFace, "glCullFace");
    UE_RESOLVE(GlPolygonMode, PfnGlPolygonMode, "glPolygonMode");
    UE_RESOLVE(GlCreateShader, PfnGlCreateShader, "glCreateShader");
    UE_RESOLVE(GlShaderSource, PfnGlShaderSource, "glShaderSource");
    UE_RESOLVE(GlCompileShader, PfnGlCompileShader, "glCompileShader");
    UE_RESOLVE(GlGetShaderiv, PfnGlGetShaderiv, "glGetShaderiv");
    UE_RESOLVE(GlGetShaderInfoLog, PfnGlGetShaderInfoLog, "glGetShaderInfoLog");
    UE_RESOLVE(GlDeleteShader, PfnGlDeleteShader, "glDeleteShader");
    UE_RESOLVE(GlCreateProgram, PfnGlCreateProgram, "glCreateProgram");
    UE_RESOLVE(GlAttachShader, PfnGlAttachShader, "glAttachShader");
    UE_RESOLVE(GlLinkProgram, PfnGlLinkProgram, "glLinkProgram");
    UE_RESOLVE(GlGetProgramiv, PfnGlGetProgramiv, "glGetProgramiv");
    UE_RESOLVE(GlGetProgramInfoLog, PfnGlGetProgramInfoLog, "glGetProgramInfoLog");
    UE_RESOLVE(GlDeleteProgram, PfnGlDeleteProgram, "glDeleteProgram");
    UE_RESOLVE(GlUseProgram, PfnGlUseProgram, "glUseProgram");
    UE_RESOLVE(GlGenVertexArrays, PfnGlGenVertexArrays, "glGenVertexArrays");
    UE_RESOLVE(GlDeleteVertexArrays, PfnGlDeleteVertexArrays, "glDeleteVertexArrays");
    UE_RESOLVE(GlBindVertexArray, PfnGlBindVertexArray, "glBindVertexArray");
    UE_RESOLVE(GlGenBuffers, PfnGlGenBuffers, "glGenBuffers");
    UE_RESOLVE(GlDeleteBuffers, PfnGlDeleteBuffers, "glDeleteBuffers");
    UE_RESOLVE(GlBindBuffer, PfnGlBindBuffer, "glBindBuffer");
    UE_RESOLVE(GlBufferData, PfnGlBufferData, "glBufferData");
    UE_RESOLVE(GlVertexAttribPointer, PfnGlVertexAttribPointer, "glVertexAttribPointer");
    UE_RESOLVE(GlEnableVertexAttribArray, PfnGlEnableVertexAttribArray, "glEnableVertexAttribArray");
    UE_RESOLVE(GlDisableVertexAttribArray, PfnGlDisableVertexAttribArray, "glDisableVertexAttribArray");
    UE_RESOLVE(GlDrawArrays, PfnGlDrawArrays, "glDrawArrays");
    UE_RESOLVE(GlDrawElements, PfnGlDrawElements, "glDrawElements");
    UE_RESOLVE(GlGetUniformLocation, PfnGlGetUniformLocation, "glGetUniformLocation");
    UE_RESOLVE(GlUniformMatrix4fv, PfnGlUniformMatrix4fv, "glUniformMatrix4fv");
    UE_RESOLVE(GlUniformMatrix3fv, PfnGlUniformMatrix3fv, "glUniformMatrix3fv");
    UE_RESOLVE(GlUniform4fv, PfnGlUniform4fv, "glUniform4fv");
    UE_RESOLVE(GlUniform3fv, PfnGlUniform3fv, "glUniform3fv");
    UE_RESOLVE(GlUniform1i, PfnGlUniform1i, "glUniform1i");
    UE_RESOLVE(GlUniform1f, PfnGlUniform1f, "glUniform1f");
    UE_RESOLVE(GlGenTextures, PfnGlGenTextures, "glGenTextures");
    UE_RESOLVE(GlDeleteTextures, PfnGlDeleteTextures, "glDeleteTextures");
    UE_RESOLVE(GlBindTexture, PfnGlBindTexture, "glBindTexture");
    UE_RESOLVE(GlTexImage2D, PfnGlTexImage2D, "glTexImage2D");
    UE_RESOLVE(GlTexParameteri, PfnGlTexParameteri, "glTexParameteri");
    UE_RESOLVE(GlActiveTexture, PfnGlActiveTexture, "glActiveTexture");
    UE_RESOLVE(GlReadPixels, PfnGlReadPixels, "glReadPixels");
    UE_RESOLVE(GlGenFramebuffers, PfnGlGenFramebuffers, "glGenFramebuffers");
    UE_RESOLVE(GlBindFramebuffer, PfnGlBindFramebuffer, "glBindFramebuffer");
    UE_RESOLVE(GlFramebufferTexture2D, PfnGlFramebufferTexture2D, "glFramebufferTexture2D");
    UE_RESOLVE(GlCheckFramebufferStatus, PfnGlCheckFramebufferStatus, "glCheckFramebufferStatus");
    UE_RESOLVE(GlDeleteFramebuffers, PfnGlDeleteFramebuffers, "glDeleteFramebuffers");
    UE_RESOLVE(GlGenRenderbuffers, PfnGlGenRenderbuffers, "glGenRenderbuffers");
    UE_RESOLVE(GlBindRenderbuffer, PfnGlBindRenderbuffer, "glBindRenderbuffer");
    UE_RESOLVE(GlRenderbufferStorage, PfnGlRenderbufferStorage, "glRenderbufferStorage");
    UE_RESOLVE(GlFramebufferRenderbuffer, PfnGlFramebufferRenderbuffer, "glFramebufferRenderbuffer");
    UE_RESOLVE(GlDeleteRenderbuffers, PfnGlDeleteRenderbuffers, "glDeleteRenderbuffers");
    UE_RESOLVE(GlGetError, PfnGlGetError, "glGetError");

    loaded_ = true;
    return true;
}

#undef UE_RESOLVE

} // namespace ue::render