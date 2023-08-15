#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"
#include "pipeline.hpp"

namespace vkn {

struct CommandPool {
    VkCommandPool handle;

    CommandPool();

    ~CommandPool();

    CommandPool(const CommandPool &other) = delete;
    CommandPool(CommandPool &&other) {
	*this = std::move(other);
    }
    CommandPool &operator=(const CommandPool &other) = delete;
    CommandPool &operator=(CommandPool &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct CommandBuffer {
    VkCommandBuffer handle;

    explicit CommandBuffer(const CommandPool &pool);

    ~CommandBuffer() = default;
    CommandBuffer(const CommandBuffer &other) = delete;
    CommandBuffer(CommandBuffer &&other) {
	*this = std::move(other);
    }
    CommandBuffer &operator=(const CommandBuffer &other) = delete;
    CommandBuffer &operator=(CommandBuffer &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    void record(
	u32 image_index,
	std::shared_ptr<vkn::Pipeline> pipeline);

    void reset();
};

vkn::CommandBuffer cmd_begin_single();

void cmd_end_single(vkn::CommandBuffer &command_buffer);

}
