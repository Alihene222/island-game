#include "global.hpp"

Global global;

int main(UNUSED int agc, UNUSED char **argv) {
    global.debug = true;

    Platform platform;
    platform.window = std::make_unique<GLFWWindow>(
	"Hello, World!", 800, 600);
    global.platform = &platform;

    Timer timer;
    global.timer = &timer;

    // Global vulkan state. Initialized in Renderer
    VkGlobal vk_global;
    global.vk_global = &vk_global;

    Renderer renderer;
    global.renderer = &renderer;

    StateGame game;
    global.game = &game;
    global.state = global.game;
    global.state->init();

    timer.init();

    while(!platform.window->is_close_requested()) {
	global.state->update();
	
	timer.tick([&]() {
	    global.state->tick();
	});

	global.state->render();
	timer.frames++;

	platform.window->end_frame();
    }

    return 0;
}
