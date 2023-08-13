#include "buffers.hpp"

#include "global.hpp"
#include "command_buffer.hpp"

u32 vkn::find_memory_type(
    u32 type_filter,
    VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(
	global.vk_global->physical_device, &mem_properties);
    for(u32 i = 0; i < mem_properties.memoryTypeCount; i++) {
	if((type_filter & (1 << i))
	    && (mem_properties.memoryTypes[i].propertyFlags
		& properties) == properties)
	return i;
    }

    log("Failed to find suitable memory type", LOG_LEVEL_ERROR);
    std::exit(-1);
}

void vkn::make_buffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer *buffer,
    VkDeviceMemory *memory) {
    VkBufferCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(
	global.vk_global->device->handle,
	&create_info,
	nullptr,
	buffer) != VK_SUCCESS) {
	log("Failed to create buffer", LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(
	global.vk_global->device->handle,
	*buffer,
	&mem_requirements);

    VkMemoryAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
	mem_requirements.memoryTypeBits,
	properties);

    if(vkAllocateMemory(
	global.vk_global->device->handle,
	&alloc_info,
	nullptr,
	memory) != VK_SUCCESS) {
	log("Failed to allocate buffer memory", LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    vkBindBufferMemory(
	global.vk_global->device->handle,
	*buffer,
	*memory,
	0);
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
