#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"

struct VKDevice {
    VkDevice handle;

    VkQueue queue_graphics;

    VkQueue queue_present;

    VKDevice();
    
    ~VKDevice();

    VKDevice(const VKDevice &other) = delete;
    VKDevice(VKDevice &&other) {
	*this = std::move(other);
    }
    VKDevice &operator=(const VKDevice &other) = delete;
    VKDevice &operator=(VKDevice &&other) {
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
