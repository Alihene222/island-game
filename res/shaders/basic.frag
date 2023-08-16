#version 450

layout(binding = 2) uniform sampler2D tex;

layout(binding = 1) uniform UBO {
    float brightness;
} ubo;

layout(location = 0) in vec3 v_color;
layout(location = 1) in vec2 v_tex_coords;

layout(location = 0) out vec4 color;

void main() {
    color = texture(tex, v_tex_coords) * ubo.brightness;
}
