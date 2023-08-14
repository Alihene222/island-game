#pragma once

#include <vk_mem_alloc.h>

#include "util/std.hpp"
#include "gfx/gfx.hpp"

namespace vkn {

struct Allocator {
    struct Entry {
	VkBuffer buffer;
	VmaAllocation allocation;
    };

    std::vector<Entry> entries;

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
};

};
