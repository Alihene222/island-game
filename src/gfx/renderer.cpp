#include "renderer.hpp"

#include "global.hpp"

Renderer::~Renderer() {

}

void Renderer::init() {
    global.vk_global->instance =
	std::make_unique<vkn::Instance>();

    global.vk_global->surface =
	global.platform->window->create_surface();

    global.vk_global->physical_device =
	vkn::pick_physical_device();

    global.vk_global->device =
	std::make_unique<vkn::Device>();

    global.vk_global->swapchain =
	std::make_unique<vkn::Swapchain>(
	    vkn::Swapchain::SRGB,
	    vkn::Swapchain::MAILBOX);
}

void Renderer::render() {

}
