#pragma once

#include "gfx/gfx.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include "device.hpp"
#include "swapchain.hpp"

struct VkGlobal {
    std::unique_ptr<vkn::Instance> instance;
    
    VkSurfaceKHR surface;

    VkPhysicalDevice physical_device;

    std::unique_ptr<vkn::Device> device;

    std::unique_ptr<vkn::Swapchain> swapchain;

    VkGlobal() = default;

    ~VkGlobal() {
	this->swapchain.reset();
	vkDestroySurfaceKHR(
	    this->instance->handle, this->surface, nullptr);
    };

    VkGlobal(const VkGlobal &other) = delete;

    VkGlobal(VkGlobal &&other) = default;

    VkGlobal &operator=(const VkGlobal &other) = delete;

    VkGlobal &operator=(VkGlobal &&other) = default;
};
