#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;

layout(location = 0) out vec3 v_color;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_pos, 1.0);
    v_color = a_color;

    gl_Position.y = -gl_Position.y;
}
