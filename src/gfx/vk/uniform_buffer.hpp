#pragma once

#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"
#include "file_texture.hpp"

namespace vkn {

struct UniformBuffer {
    VkBuffer handle;

    VmaAllocation alloc;

    void *data;

    UniformBuffer() = default;
    explicit UniformBuffer(usize size);

    ~UniformBuffer();

    UniformBuffer(const UniformBuffer &other) = delete;
    UniformBuffer(UniformBuffer &&other) {
	*this = std::move(other);
    }
    UniformBuffer &operator=(const UniformBuffer &other) = delete;
    UniformBuffer &operator=(UniformBuffer &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	this->alloc = other.alloc;
	other.alloc = VK_NULL_HANDLE;
	this->data = other.data;
	other.data = nullptr;
	return *this;
    }
};

struct DescriptorSetLayout {
    VkDescriptorSetLayout handle;

    enum Stage {
	STAGE_VERTEX,
	STAGE_FRAGMENT
    } stage;

    DescriptorSetLayout() = default;
    DescriptorSetLayout(Stage stage);

    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout &other) = delete;
    DescriptorSetLayout(DescriptorSetLayout &&other) {
	*this = std::move(other);
    }
    DescriptorSetLayout &operator=(const DescriptorSetLayout &other) = delete;
    DescriptorSetLayout &operator=(DescriptorSetLayout &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct DescriptorPool {
    VkDescriptorPool handle;

    DescriptorPool() = default;
    DescriptorPool(u32 frames_in_flight);

    ~DescriptorPool();

    DescriptorPool(const DescriptorPool &other) = delete;
    DescriptorPool(DescriptorPool &&other) {
	*this = std::move(other);
    }
    DescriptorPool &operator=(const DescriptorPool &other) = delete;
    DescriptorPool &operator=(DescriptorPool &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct DescriptorSet {
    VkDescriptorSet handle;

    DescriptorSet() = default;
    DescriptorSet(
	const vkn::UniformBuffer &uniform_buffer,
	const vkn::DescriptorSetLayout &layout,
	const vkn::DescriptorPool &pool,
	usize ubo_size,
	const FileTexture &texture);

    ~DescriptorSet() = default;
    DescriptorSet(const DescriptorSet &other) = delete;
    DescriptorSet(DescriptorSet &&other) {
	*this = std::move(other);
    }
    DescriptorSet &operator=(const DescriptorSet &other) = delete;
    DescriptorSet &operator=(DescriptorSet &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

}
