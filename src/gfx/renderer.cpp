#include "renderer.hpp"

#include "global.hpp"

Renderer::Renderer() {
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

    this->pipelines["main"] =
	std::make_shared<vkn::Pipeline>(
	    "res/shaders/basic.vert.spirv",
	    "res/shaders/basic.frag.spirv");

    global.vk_global->swapchain->create_framebuffers(
	this->pipelines["main"]->render_pass);

    this->command_pool = std::make_unique<vkn::CommandPool>();

    this->command_buffer =
	std::make_unique<vkn::CommandBuffer>(
	    *this->command_pool);
}

Renderer::~Renderer() {

}

void Renderer::render() {

}
