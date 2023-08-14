#include "command_buffer.hpp"

#include "global.hpp"

vkn::CommandPool::CommandPool() {
    vkn::QueueFamilyIndices indices =
	vkn::find_queue_families(
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

vkn::CommandPool::~CommandPool() {
    vkDestroyCommandPool(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

vkn::CommandBuffer::CommandBuffer(const CommandPool &pool) {
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

void vkn::CommandBuffer::record(
    u32 image_index,
    std::shared_ptr<vkn::Pipeline> pipeline) {
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
    render_pass_info->renderPass = pipeline->render_pass;
    render_pass_info->framebuffer =
	global.vk_global->swapchain->framebuffers[image_index];
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
	pipeline->handle);

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

    VkBuffer vertex_buffers[] = {
	global.renderer->vertex_buffer->handle
    };
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(
	this->handle,
	0, 1,
	vertex_buffers,
	offsets);

    vkCmdBindDescriptorSets(
	this->handle,
	VK_PIPELINE_BIND_POINT_GRAPHICS,
	global.renderer->pipelines["main"]->layout,
	0,
	1,
	&global.renderer->descriptor_sets
	    [global.renderer->current_frame]->handle,
	0,
	nullptr);

    vkCmdDraw(this->handle, 3, 1, 0, 0);

    vkCmdEndRenderPass(this->handle);
    
    if(vkEndCommandBuffer(this->handle) != VK_SUCCESS) {
	log(
	    "Failed to end recording command buffer",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }
}

void vkn::CommandBuffer::reset() {
    vkResetCommandBuffer(this->handle, 0);
}
