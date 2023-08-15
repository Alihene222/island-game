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

    usize size;

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

    VkDescriptorBufferInfo descriptor_info();
};

struct DescriptorSetLayout {
    enum Stage {
	STAGE_VERTEX,
	STAGE_FRAGMENT
    };

    struct Binding {
	u32 location;
	Stage stage;
	VkDescriptorType type;
    };

    VkDescriptorSetLayout handle;

    DescriptorSetLayout() = default;
    explicit DescriptorSetLayout(
	const std::vector<Binding> &bindings);

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
    struct Info {
	usize size = 0;
	std::vector<VkWriteDescriptorSet> descriptor_writes;

	inline void clear() {
	    this->descriptor_writes.clear();
	}
    } info;

    VkDescriptorSet handle;

    DescriptorSet() = default;
    DescriptorSet(
	const vkn::DescriptorSetLayout &layout,
	const vkn::DescriptorPool &pool);

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

    DescriptorSet &add_uniform(VkDescriptorBufferInfo *info);
    DescriptorSet &add_sampler(VkDescriptorImageInfo *info);
    void submit();
};

}
