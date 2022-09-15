#version 450

layout (location = 0) in vec3 position;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 uv;

layout (push_constant) uniform Push {
    mat4 transform;
    vec3 color;
} push;

void main () {
    gl_Position = vec4(push.transform * vec4(position, 1.0));
    fragColor = push.color;
    uv = vec2(0.0, 0.0);
}
