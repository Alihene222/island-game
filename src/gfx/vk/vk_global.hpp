#pragma once

#include "gfx/gfx.hpp"
#include "vk_instance.hpp"
#include "vk_physical_device.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"
#include "vk_command_buffer.hpp"
#include "vk_allocator.hpp"
#include "vk_uniform_buffer.hpp"

struct VKGlobal {
    std::unique_ptr<VKInstance> instance;
    
    VkSurfaceKHR surface;

    VkPhysicalDevice physical_device;

    std::unique_ptr<VKDevice> device;

    std::unique_ptr<VKSwapchain> swapchain;

    std::unique_ptr<VKCommandPool> command_pool;

    std::unique_ptr<VKAllocator> allocator;

    VKDescriptorAllocator descriptor_allocator;

    VKDescriptorLayoutCache descriptor_cache;

    VKGlobal() = default;

    ~VKGlobal() {
	this->descriptor_allocator.cleanup();
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
