#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"

struct Mouse {
    f32 x, y;
    std::function<void()> on_click;
};

struct GLFWWindow {
    GLFWwindow *window;

    u16 width;
    u16 height;

    f32 aspect_ratio;

    GLFWWindow(std::string name, u16 width, u16 height);

    ~GLFWWindow();

    GLFWWindow(const GLFWWindow &other) : window(other.window) {}
    GLFWWindow(GLFWWindow &&other) {
	other.window = nullptr;
    }
    GLFWWindow &operator=(const GLFWWindow &other) {
	return *this = GLFWWindow(other);
    }
    GLFWWindow &operator=(GLFWWindow &&other) {
	this->window = other.window;
	other.window = nullptr;
	return *this;
    }

    void end_frame();

    bool is_close_requested();

    bool is_key_pressed(u32 key);
};

struct Platform {
    std::unique_ptr<GLFWWindow> window;

    Mouse mouse;
};
