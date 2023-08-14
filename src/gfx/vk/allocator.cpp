#include "allocator.hpp"

#include "global.hpp"

vkn::Allocator::Allocator() {
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

vkn::Allocator::~Allocator() {
    for(const auto &entry : this->entries) {
	vmaDestroyBuffer(
	    this->handle,
	    entry.buffer,
	    entry.allocation);
    }
    vmaDestroyAllocator(this->handle);
}

void vkn::Allocator::add_entry(
    VkBuffer buffer,
    VmaAllocation alloc) {
    Entry entry { buffer, alloc };
    this->entries.push_back(entry);
}
