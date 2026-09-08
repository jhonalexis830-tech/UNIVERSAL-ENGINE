#pragma once

#include <string>

namespace ue::render {

/// Fuentes de los shaders incluidos con el motor.
/// Los usuarios pueden reemplazarlos (útil para personalizar el aspecto).
namespace ShaderSources {

    // Shader de malla iluminada (Blinn-Phong simplificado).
    inline const std::string kVertexLit = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUv;

void main() {
    vec4 world = uModel * vec4(aPosition, 1.0);
    vWorldPos = world.xyz;
    vNormal   = uNormalMatrix * aNormal;
    vUv       = aUv;
    gl_Position = uProjection * uView * world;
}
)GLSL";

    inline const std::string kFragmentLit = R"GLSL(
#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUv;

out vec4 oColor;

uniform vec4  uBaseColor;
uniform float uShininess;
uniform bool  uHasTexture;
uniform sampler2D uAlbedo;

uniform vec3 uSunDirection;
uniform vec3 uSunColor;
uniform vec3 uAmbient;
uniform vec3 uCameraPos;

void main() {
    vec3 albedo = uBaseColor.rgb;
    float alpha = uBaseColor.a;
    if (uHasTexture) {
        albedo *= texture(uAlbedo, vUv).rgb;
    }

    vec3 n = normalize(vNormal);
    vec3 l = normalize(-uSunDirection);
    vec3 v = normalize(uCameraPos - vWorldPos);
    vec3 h = normalize(l + v);

    float diff = max(dot(n, l), 0.0);
    float spec = pow(max(dot(n, h), 0.0), uShininess) * step(0.001, diff);

    vec3 color = albedo * (uAmbient + uSunColor * diff) + uSunColor * spec * 0.45;
    oColor = vec4(color, alpha);
}
)GLSL";

    // Shader sin iluminación (rejilla, ayuda, overlays).
    inline const std::string kVertexUnlit = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
)GLSL";

    inline const std::string kFragmentUnlit = R"GLSL(
#version 330 core

out vec4 oColor;
uniform vec4 uBaseColor;

void main() {
    oColor = uBaseColor;
}
)GLSL";

} // namespace ShaderSources
} // namespace ue::render