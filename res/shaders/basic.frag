#version 450

layout(binding = 1) uniform sampler2D tex;

layout(location = 0) in vec3 v_color;
layout(location = 1) in vec2 v_tex_coords;

layout(location = 0) out vec4 color;

void main() {
    color = texture(tex, v_tex_coords);
}
