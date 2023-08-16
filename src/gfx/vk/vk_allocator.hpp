#pragma once

#include <vk_mem_alloc.h>

#include "util/std.hpp"
#include "gfx/gfx.hpp"

struct VKAllocator {
    struct BufferEntry {
	VkBuffer buffer;
	VmaAllocation allocation;
    };

    struct ImageEntry {
	VkImage image;
	VmaAllocation allocation;
    };

    std::vector<BufferEntry> buffer_entries;

    std::vector<ImageEntry> image_entries;

    VmaAllocator handle;

    VKAllocator();

    ~VKAllocator();

    VKAllocator(const VKAllocator &other) = delete;
    VKAllocator(VKAllocator &&other) {
	*this = std::move(other);
    }
    VKAllocator &operator=(const VKAllocator &other) = delete;
    VKAllocator &operator=(VKAllocator &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    void add_entry(VkBuffer buffer, VmaAllocation alloc);
    void add_entry(VkImage image, VmaAllocation alloc);
};
