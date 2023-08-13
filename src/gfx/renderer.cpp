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
	    vkn::Swapchain::FIFO);

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

    this->image_available_semaphore =
	std::make_unique<vkn::Semaphore>();

    this->render_finished_semaphore =
	std::make_unique<vkn::Semaphore>();

    this->in_flight_fence = std::make_unique<vkn::Fence>();
}

Renderer::~Renderer() {
    global.vk_global->device->wait_idle();
}

void Renderer::render() {
    this->in_flight_fence->wait();
    this->in_flight_fence->reset();

    u32 image_index;
    VkResult result = vkAcquireNextImageKHR(
	global.vk_global->device->handle,
	global.vk_global->swapchain->handle,
	UINT64_MAX,
	this->image_available_semaphore->handle,
	VK_NULL_HANDLE,
	&image_index);

    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
	global.vk_global->swapchain->adapt(
	    this->pipelines["main"]->render_pass);
    } else if(result != VK_SUCCESS
	&& result != VK_SUBOPTIMAL_KHR) {
	log("Failed to adapt swapchain", LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    this->in_flight_fence->reset();

    this->command_buffer->reset();
    this->command_buffer->record(
	image_index, this->pipelines["main"]);

    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {
	this->image_available_semaphore->handle
    };
    VkPipelineStageFlags wait_stages[] = {
	VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers =
	&this->command_buffer->handle;

    VkSemaphore signal_semaphores[] = {
	this->render_finished_semaphore->handle
    };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if(vkQueueSubmit(
	global.vk_global->device->queue_graphics,
	1, &submit_info,
	this->in_flight_fence->handle) != VK_SUCCESS) {
	log(
	    "Failed to submit command buffer to graphisc queue",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    VkPresentInfoKHR present_info {};
    present_info.sType =
	VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {
	global.vk_global->swapchain->handle	
    };

    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    result = vkQueuePresentKHR(
	global.vk_global->device->queue_present,
	&present_info);
    if(result == VK_ERROR_OUT_OF_DATE_KHR
	|| result == VK_SUBOPTIMAL_KHR
	|| global.platform->window->size_changed) {
	global.vk_global->swapchain->adapt(
	    this->pipelines["main"]->render_pass);
	    global.platform->window->size_changed = false;
    } else if(result != VK_SUCCESS) {
	log(
	    "Failed to submit command buffer to present queue",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }
}
