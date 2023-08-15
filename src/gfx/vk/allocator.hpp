#pragma once

#include <vk_mem_alloc.h>

#include "util/std.hpp"
#include "gfx/gfx.hpp"

namespace vkn {

struct Allocator {
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

    Allocator();

    ~Allocator();

    Allocator(const Allocator &other) = delete;
    Allocator(Allocator &&other) {
	*this = std::move(other);
    }
    Allocator &operator=(const Allocator &other) = delete;
    Allocator &operator=(Allocator &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    void add_entry(VkBuffer buffer, VmaAllocation alloc);
    void add_entry(VkImage image, VmaAllocation alloc);
};

};
