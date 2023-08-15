#pragma once

#include "gfx/gfx.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "command_buffer.hpp"
#include "allocator.hpp"

struct VKGlobal {
    std::unique_ptr<VKInstance> instance;
    
    VkSurfaceKHR surface;

    VkPhysicalDevice physical_device;

    std::unique_ptr<VKDevice> device;

    std::unique_ptr<VKSwapchain> swapchain;

    std::unique_ptr<VKCommandPool> command_pool;

    std::unique_ptr<VKAllocator> allocator;

    VKGlobal() = default;

    ~VKGlobal() {
	this->swapchain.reset();
	vkDestroySurfaceKHR(
	    this->instance->handle, this->surface, nullptr);
	allocator.reset();
    };

    VKGlobal(const VKGlobal &other) = delete;
    VKGlobal(VKGlobal &&other) = default;
    VKGlobal &operator=(const VKGlobal &other) = delete;
    VKGlobal &operator=(VKGlobal &&other) = default;
};
