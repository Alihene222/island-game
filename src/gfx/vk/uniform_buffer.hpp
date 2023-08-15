#pragma once

#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"
#include "file_texture.hpp"

struct VKUniformBuffer {
    VkBuffer handle;

    VmaAllocation alloc;

    void *data;

    usize size;

    VKUniformBuffer() = default;
    explicit VKUniformBuffer(usize size);

    ~VKUniformBuffer();

    VKUniformBuffer(const VKUniformBuffer &other) = delete;
    VKUniformBuffer(VKUniformBuffer &&other) {
	*this = std::move(other);
    }
    VKUniformBuffer &operator=(const VKUniformBuffer &other) = delete;
    VKUniformBuffer &operator=(VKUniformBuffer &&other) {
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

struct VKDescriptorSetLayout {
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

    VKDescriptorSetLayout() = default;
    explicit VKDescriptorSetLayout(
	const std::vector<Binding> &bindings);

    ~VKDescriptorSetLayout();

    VKDescriptorSetLayout(const VKDescriptorSetLayout &other) = delete;
    VKDescriptorSetLayout(VKDescriptorSetLayout &&other) {
	*this = std::move(other);
    }
    VKDescriptorSetLayout &operator=(const VKDescriptorSetLayout &other) = delete;
    VKDescriptorSetLayout &operator=(VKDescriptorSetLayout &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct VKDescriptorPool {
    VkDescriptorPool handle;

    VKDescriptorPool() = default;
    VKDescriptorPool(u32 frames_in_flight);

    ~VKDescriptorPool();

    VKDescriptorPool(const VKDescriptorPool &other) = delete;
    VKDescriptorPool(VKDescriptorPool &&other) {
	*this = std::move(other);
    }
    VKDescriptorPool &operator=(const VKDescriptorPool &other) = delete;
    VKDescriptorPool &operator=(VKDescriptorPool &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct VKDescriptorSet {
    struct Info {
	usize size = 0;
	std::vector<VkWriteDescriptorSet> descriptor_writes;

	inline void clear() {
	    this->descriptor_writes.clear();
	}
    } info;

    VkDescriptorSet handle;

    VKDescriptorSet() = default;
    VKDescriptorSet(
	const VKDescriptorSetLayout &layout,
	const VKDescriptorPool &pool);

    ~VKDescriptorSet() = default;
    VKDescriptorSet(const VKDescriptorSet &other) = delete;
    VKDescriptorSet(VKDescriptorSet &&other) {
	*this = std::move(other);
    }
    VKDescriptorSet &operator=(const VKDescriptorSet &other) = delete;
    VKDescriptorSet &operator=(VKDescriptorSet &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    VKDescriptorSet &add_uniform(VkDescriptorBufferInfo *info);
    VKDescriptorSet &add_sampler(VkDescriptorImageInfo *info);
    void submit();
};
