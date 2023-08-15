#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "buffers.hpp"

struct VKFileTexture {
    VkBuffer buffer;
    VmaAllocation alloc;

    VkImage image;
    VmaAllocation image_alloc;

    VkImageView image_view;

    VkSampler sampler;

    VKFileTexture(std::string path);

    ~VKFileTexture();

    VKFileTexture(const VKFileTexture &other) = delete;
    VKFileTexture(VKFileTexture &&other) { *this = std::move(other); }
    VKFileTexture &operator=(const VKFileTexture &other) = delete;
    VKFileTexture &operator=(VKFileTexture &&other) {
	this->buffer = other.buffer;
	other.buffer = VK_NULL_HANDLE;
	this->alloc = other.alloc;
	other.alloc = VK_NULL_HANDLE;
	this->image = other.image;
	other.image = VK_NULL_HANDLE;
	this->image_alloc = other.image_alloc;
	other.image_alloc = VK_NULL_HANDLE;
	return *this;
    }
    
    VkDescriptorImageInfo descriptor_info();
};
