#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"

namespace vkn {

struct Device {
    VkDevice handle;

    VkQueue queue_graphics;

    VkQueue queue_present;

    Device();
    
    ~Device();

    Device(const Device &other) = delete;
    Device(Device &&other) {
	*this = std::move(other);
    }
    Device &operator=(const Device &other) = delete;
    Device &operator=(Device &&other) {
	this->handle = other.handle;
	this->queue_graphics = other.queue_graphics;
	this->queue_present = other.queue_present;
	other.handle = VK_NULL_HANDLE;
	other.queue_graphics = VK_NULL_HANDLE;
	other.queue_present = VK_NULL_HANDLE;
	return *this;
    }

    void wait_idle();
};

}
