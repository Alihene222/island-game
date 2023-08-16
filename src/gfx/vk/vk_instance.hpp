#pragma once

#include "gfx//gfx.hpp"
#include "util/std.hpp"

struct VKInstance {
    VkInstance handle;

    VkDebugUtilsMessengerEXT debug_messenger;

    VKInstance();

    ~VKInstance();

    VKInstance(const VKInstance &other) = delete;
    VKInstance(VKInstance &&other) {
	*this = std::move(other);
    }
    VKInstance &operator=(const VKInstance &other) = delete;
    VKInstance &operator=(VKInstance &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};
