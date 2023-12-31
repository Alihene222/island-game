#include "vk_allocator.hpp"

#include "global.hpp"

VKAllocator::VKAllocator() {
    VmaAllocatorCreateInfo create_info {};
    create_info.physicalDevice =
	global.vk_global->physical_device;
    create_info.device =
	global.vk_global->device->handle;
    create_info.instance =
	global.vk_global->instance->handle;

    if(vmaCreateAllocator(
	&create_info,
	&this->handle) != VK_SUCCESS) {
	log("Failed to create vulkan allocator", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

VKAllocator::~VKAllocator() {
    for(const auto &entry : this->buffer_entries) {
	vmaDestroyBuffer(
	    this->handle,
	    entry.buffer,
	    entry.allocation);
    }
    for(const auto &entry : this->image_entries) {
	vmaDestroyImage(
	    this->handle,
	    entry.image,
	    entry.allocation);
    }
    vmaDestroyAllocator(this->handle);
}

void VKAllocator::add_entry(
    VkBuffer buffer,
    VmaAllocation alloc) {
    BufferEntry entry { buffer, alloc };
    this->buffer_entries.push_back(entry);
}

void VKAllocator::add_entry(
    VkImage image,
    VmaAllocation alloc) {
    ImageEntry entry { image, alloc };
    this->image_entries.push_back(entry);
}
