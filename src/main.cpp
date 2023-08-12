#include "global.hpp"

Global global;

int main() {
    Platform platform;
    platform.window = std::make_unique<GLFWWindow>(
	"Hello, World!", 800, 600);
    global.platform = &platform;

    Timer timer;
    global.timer = &timer;

    Renderer renderer;
    global.renderer = &renderer;

    StateGame game;
    global.game = &game;
    global.state = global.game;
    global.state->init();

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
