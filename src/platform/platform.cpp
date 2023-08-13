#include "platform.hpp"

#include "global.hpp"

static void glfw_error_callback(
    UNUSED int err,
    const char *msg) {
    log(std::string(msg), LOG_LEVEL_FATAL);
}

static void framebuffer_size_callback(
    GLFWwindow *w,
    UNUSED i32 width,
    UNUSED i32 height) {
    GLFWWindow *window = reinterpret_cast<GLFWWindow*>(w);
    window->size_changed = true;
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

    glfwSetWindowUserPointer(this->window, this);
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

VkSurfaceKHR GLFWWindow::create_surface() {
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(
	global.vk_global->instance->handle,
	this->window,
	nullptr,
	&surface) != VK_SUCCESS) {
	log("Failed to create window surface", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
    return surface;
}

std::tuple<i32, i32> GLFWWindow::get_size() {
    i32 width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    return std::tuple<i32, u32>(width, height);
}
