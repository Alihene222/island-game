#include "platform.hpp"

static void glfw_error_callback(int err, const char *msg) {
    log(std::string(msg), LOG_LEVEL_FATAL);
}

GLFWWindow::GLFWWindow(std::string name, u16 width, u16 height)
    : width(width),
    height(height) {
    glfwSetErrorCallback(glfw_error_callback);

    if(!glfwInit()) {
	log("Failed to initialize GLFW", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    this->window = glfwCreateWindow(
	width, height, name.c_str(), nullptr, nullptr);
    if(!this->window) {
	log(
	    "Failed to create window", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

GLFWWindow::~GLFWWindow() {
    glfwTerminate();
}

void GLFWWindow::end_frame() {
    glfwPollEvents();
}

bool GLFWWindow::is_close_requested() {
    return glfwWindowShouldClose(this->window);
}

bool GLFWWindow::is_key_pressed(u32 key) {
    return glfwGetKey(this->window, key) == GLFW_PRESS;
}

std::tuple<u32, const char**> GLFWWindow::get_extension_info() {
    u32 extension_count = 0;
    const char **extensions =
	glfwGetRequiredInstanceExtensions(&extension_count);
    
    return std::tuple<u32, const char**>(
	extension_count, extensions);
}
