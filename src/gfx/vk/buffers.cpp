#include "buffers.hpp"

#include "global.hpp"
#include "command_buffer.hpp"

void vkn::make_buffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    i32 flags,
    VkBuffer *buffer,
    VmaAllocation *alloc,
    f32 priority) {
    VkBufferCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info {};
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = flags;
    alloc_create_info.priority = priority;

    VmaAllocationInfo alloc_info {};

    vmaCreateBuffer(
	global.vk_global->allocator->handle,
	&create_info,
	&alloc_create_info,
	buffer,
	alloc,
	&alloc_info);
}

void vkn::copy_buffer(
    VkBuffer src,
    VkBuffer dst,
    VkDeviceSize size) {
    vkn::CommandBuffer command_buffer(
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
	    "Failed to begin copying buffer",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    VkBufferCopy copy_region {};
    copy_region.size = size;
    vkCmdCopyBuffer(
	command_buffer.handle,
	src, dst, 1,
	&copy_region);

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
	1,
	&submit_info,
	VK_NULL_HANDLE);
    vkQueueWaitIdle(
	global.vk_global->device->queue_graphics);

    vkFreeCommandBuffers(
	global.vk_global->device->handle,
	global.vk_global->command_pool->handle,
	1,
	&command_buffer.handle);
}
