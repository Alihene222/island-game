#pragma once

#include "gfx//gfx.hpp"
#include "util/std.hpp"

namespace vkn {

struct Instance {
    VkInstance handle;

    VkDebugUtilsMessengerEXT debug_messenger;

    Instance();

    ~Instance();

    Instance(const Instance &other) = delete;

    Instance(Instance &&other) {
	*this = std::move(other);
    }

    Instance &operator=(const Instance &other) = delete;

    Instance &operator=(Instance &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

}
