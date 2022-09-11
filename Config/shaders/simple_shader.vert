#version 450

layout (location = 0) in vec3 position;

layout (location = 0) out vec3 fragColor;

layout (push_constant) uniform Push {
    vec3 translation;
    vec3 rotation;
    vec3 scale;
} push;

void main () {
    vec3 translation = position + push.translation;
    gl_Position = vec4(translation, 1.0);
    fragColor = vec3(1.0, 0.0, 1.0);
}
