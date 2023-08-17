#include "vk_buffers.hpp"

#include "global.hpp"
#include "vk_command_buffer.hpp"

void make_buffer(
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

void copy_buffer(
    VkBuffer src,
    VkBuffer dst,
    VkDeviceSize size) {
    VKCommandBuffer command_buffer =
	cmd_begin_single();

    VkBufferCopy copy_region {};
    copy_region.size = size;
    vkCmdCopyBuffer(
	command_buffer.handle,
	src, dst, 1,
	&copy_region);

    cmd_end_single(command_buffer);
}

void make_image(
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImage *image,
    VmaAllocation *alloc,
    VmaAllocationCreateFlags mem_flags) {
    VkImageCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.extent.depth = 1;
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.format = format;
    create_info.tiling = tiling;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo alloc_create_info {};
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = mem_flags;

    if(vmaCreateImage(
	global.vk_global->allocator->handle,
	&create_info,
	&alloc_create_info,
	image,
	alloc,
	nullptr) != VK_SUCCESS) {
	log("Failed to create image", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}
