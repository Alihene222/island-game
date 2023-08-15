#include "command_buffer.hpp"

#include "global.hpp"

VKCommandPool::VKCommandPool() {
    QueueFamilyIndices indices =
	find_queue_families(
	    global.vk_global->physical_device);

    VkCommandPoolCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags =
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = indices.graphics.value();

    if(vkCreateCommandPool(
	global.vk_global->device->handle,
	&create_info, nullptr,
	&this->handle) != VK_SUCCESS) {
	log("Failed to create command pool", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

VKCommandPool::~VKCommandPool() {
    vkDestroyCommandPool(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

VKCommandBuffer::VKCommandBuffer(const VKCommandPool &pool) {
    VkCommandBufferAllocateInfo alloc_info {};
    alloc_info.sType =
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool.handle;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(
	global.vk_global->device->handle,
	&alloc_info, &this->handle) != VK_SUCCESS) {
	log("Failed to allocate command buffer", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

void VKCommandBuffer::begin(
    const VKPipeline &pipeline,
    VkFramebuffer framebuffer) {
    VkCommandBufferBeginInfo *begin_info =
	global.frame_allocator.calloc<VkCommandBufferBeginInfo>();
    begin_info->sType =
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(
	this->handle,
	begin_info) != VK_SUCCESS) {
	log(
	    "Failed to begin recording command buffer",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    VkRenderPassBeginInfo *render_pass_info =
	global.frame_allocator.calloc<VkRenderPassBeginInfo>();
    render_pass_info->sType =
	VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info->renderPass = pipeline.render_pass;
    render_pass_info->framebuffer = framebuffer;
    render_pass_info->renderArea.offset = { 0, 0 };
    render_pass_info->renderArea.extent =
	global.vk_global->swapchain->extent;

    VkClearValue *clear_color =
	global.frame_allocator.alloc<VkClearValue>();
    *clear_color = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
    render_pass_info->clearValueCount = 1;
    render_pass_info->pClearValues = clear_color;

    vkCmdBeginRenderPass(
	this->handle,
	render_pass_info,
	VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
	this->handle,
	VK_PIPELINE_BIND_POINT_GRAPHICS,
	pipeline.handle);

    VkViewport *viewport = global.frame_allocator.calloc<VkViewport>();
    viewport->x = 0.0f;
    viewport->y = 0.0f;
    viewport->width =
	global.vk_global->swapchain->extent.width;
    viewport->height =
	global.vk_global->swapchain->extent.height;
    viewport->minDepth = 0.0f;
    viewport->maxDepth = 1.0f;
    vkCmdSetViewport(this->handle, 0, 1, viewport);

    VkRect2D *scissor = global.frame_allocator.calloc<VkRect2D>();
    scissor->offset = { 0, 0 };
    scissor->extent = global.vk_global->swapchain->extent;
    vkCmdSetScissor(this->handle, 0, 1, scissor);
}

void VKCommandBuffer::end() {
    vkCmdEndRenderPass(this->handle);
    
    if(vkEndCommandBuffer(this->handle) != VK_SUCCESS) {
	log(
	    "Failed to end recording command buffer",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }
}

void VKCommandBuffer::reset() {
    vkResetCommandBuffer(this->handle, 0);
}

VKCommandBuffer cmd_begin_single() {
    VKCommandBuffer command_buffer(
	*global.vk_global->command_pool);

    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType =
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags =
	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if(vkBeginCommandBuffer(
	command_buffer.handle,
	&begin_info) != VK_SUCCESS) {
	log(
	    "Failed to begin single time command buffer",
	    LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    return command_buffer;
}

void cmd_end_single(VKCommandBuffer &command_buffer) {
    if(vkEndCommandBuffer(
	command_buffer.handle) != VK_SUCCESS) {
	log(
	    "Failed to end copying buffer",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }
    
    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers =
	&command_buffer.handle;

    vkQueueSubmit(
	global.vk_global->device->queue_graphics,
	1, &submit_info,
	VK_NULL_HANDLE);
    vkQueueWaitIdle(
	global.vk_global->device->queue_graphics);

    vkFreeCommandBuffers(
	global.vk_global->device->handle,
	global.vk_global->command_pool->handle,
	1, &command_buffer.handle);
}
