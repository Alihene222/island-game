$input a_position, a_color
$output v_color

#include <bgfx_shader.sh>

void main() {
    gl_Position = vec4(a_position, 1.0);
    v_color = a_color;
}
