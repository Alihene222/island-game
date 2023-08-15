#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "buffers.hpp"

namespace vkn {

struct FileTexture {
    VkBuffer buffer;
    VmaAllocation alloc;

    VkImage image;
    VmaAllocation image_alloc;

    VkImageView image_view;

    VkSampler sampler;

    FileTexture(std::string path);

    ~FileTexture();

    FileTexture(const FileTexture &other) = delete;
    FileTexture(FileTexture &&other) { *this = std::move(other); }
    FileTexture &operator=(const FileTexture &other) = delete;
    FileTexture &operator=(FileTexture &&other) {
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

}
