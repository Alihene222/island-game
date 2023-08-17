#pragma once

#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/std.hpp"

struct VKDepthBuffer {
    VkImage image;
    VmaAllocation alloc;
    VkImageView image_view;

    VKDepthBuffer();

    ~VKDepthBuffer();

    VKDepthBuffer(const VKDepthBuffer &other) = delete;
    VKDepthBuffer(VKDepthBuffer &&other) {
	*this = std::move(other);
    }
    VKDepthBuffer &operator=(const VKDepthBuffer &other) = delete;
    VKDepthBuffer &operator=(VKDepthBuffer &&other) {
	this->image = other.image;
	this->alloc = other.alloc;
	this->image_view = other.image_view;
	other.image = VK_NULL_HANDLE;
	other.alloc = VK_NULL_HANDLE;
	other.image_view = VK_NULL_HANDLE;
	return *this;
    }

    static VkFormat find_depth_format();
};
