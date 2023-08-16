#pragma once

#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"
#include "vk_file_texture.hpp"

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

struct VKDescriptorAllocator {
    struct PoolSizes {
	std::unordered_map<VkDescriptorType, float> sizes = {
	    { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
	    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
	    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
	    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
	    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
	    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
	    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
	    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
	    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
	    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
	    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
	};
    };

    std::vector<VkDescriptorPool> used_pools;
    std::vector<VkDescriptorPool> free_pools;

    VkDescriptorPool current_pool = VK_NULL_HANDLE;

    VKDescriptorAllocator() = default;

    ~VKDescriptorAllocator() = default;

    VKDescriptorAllocator(
	const VKDescriptorAllocator &other) = delete;
    VKDescriptorAllocator(VKDescriptorAllocator &&other) {
	*this = std::move(other);
    }
    VKDescriptorAllocator &operator=(
	const VKDescriptorAllocator &other) = delete;
    VKDescriptorAllocator &operator=(VKDescriptorAllocator &&other) {
	this->used_pools = other.used_pools;
	this->free_pools = other.free_pools;
	this->current_pool = other.current_pool;
	other.used_pools.clear();
	other.free_pools.clear();
	other.current_pool = VK_NULL_HANDLE;
	return *this;
    }

    VkDescriptorSet alloc(VkDescriptorSetLayout &layout);

    void reset();

    void cleanup();

private:
    PoolSizes descriptor_sizes;
    
    VkDescriptorPool get_pool();
};

struct VKDescriptorLayoutCache {
    VKDescriptorLayoutCache() = default;

    ~VKDescriptorLayoutCache();

    VKDescriptorLayoutCache(
	const VKDescriptorLayoutCache &other) = delete;
    VKDescriptorLayoutCache(VKDescriptorLayoutCache &&other) = default;
    VKDescriptorLayoutCache &operator=(
	const VKDescriptorLayoutCache &other) = delete;
    VKDescriptorLayoutCache &operator=(
	VKDescriptorLayoutCache &&other) = default;

    VkDescriptorSetLayout create_layout(
	VkDescriptorSetLayoutCreateInfo *create_info);
    
    struct DescriptorLayoutInfo {
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	bool operator==(const DescriptorLayoutInfo &other) const;

	usize hash() const;
    };

private:
    struct DescriptorLayoutHash {
	usize operator()(const DescriptorLayoutInfo &other) const {
	    return other.hash();
	}
    };

    std::unordered_map<
	DescriptorLayoutInfo,
	VkDescriptorSetLayout,
	DescriptorLayoutHash> layout_cache;

};

struct VKDescriptorBuilder {
    VKDescriptorBuilder() = default;
    ~VKDescriptorBuilder() = default;
    VKDescriptorBuilder(const VKDescriptorBuilder &other) = delete;
    VKDescriptorBuilder(VKDescriptorBuilder &&other) = default;
    VKDescriptorBuilder &operator=(
	const VKDescriptorBuilder &other) = delete;
    VKDescriptorBuilder &operator=(
	VKDescriptorBuilder &&other) = default;

    static VKDescriptorBuilder begin(
	VKDescriptorAllocator *allocator,
	VKDescriptorLayoutCache *cache);

    VKDescriptorBuilder &bind_buffer(
	u32 binding,
	VkDescriptorBufferInfo *info,
	VkDescriptorType type,
	VkShaderStageFlags stage);
    VKDescriptorBuilder &bind_image(
	u32 binding,
	VkDescriptorImageInfo *info,
	VkDescriptorType type,
	VkShaderStageFlags stage);

    void build(VkDescriptorSet &set, VkDescriptorSetLayout &layout);

private:
    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VKDescriptorAllocator* allocator;
    VKDescriptorLayoutCache *cache;
};
