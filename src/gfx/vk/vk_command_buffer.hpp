#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"
#include "vk_pipeline.hpp"

struct VKCommandPool {
    VkCommandPool handle;

    VKCommandPool();

    ~VKCommandPool();

    VKCommandPool(const VKCommandPool &other) = delete;
    VKCommandPool(VKCommandPool &&other) {
	*this = std::move(other);
    }
    VKCommandPool &operator=(const VKCommandPool &other) = delete;
    VKCommandPool &operator=(VKCommandPool &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct VKCommandBuffer {
    VkCommandBuffer handle;

    explicit VKCommandBuffer(const VKCommandPool &pool);

    ~VKCommandBuffer() = default;
    VKCommandBuffer(const VKCommandBuffer &other) = delete;
    VKCommandBuffer(VKCommandBuffer &&other) {
	*this = std::move(other);
    }
    VKCommandBuffer &operator=(const VKCommandBuffer &other) = delete;
    VKCommandBuffer &operator=(VKCommandBuffer &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    void record(
	u32 image_index,
	std::shared_ptr<VKPipeline> pipeline);

    void begin(
	const VKPipeline &pipeline,
	VkFramebuffer framebuffer);

    void end();

    void reset();
};

VKCommandBuffer cmd_begin_single();

void cmd_end_single(VKCommandBuffer &command_buffer);
