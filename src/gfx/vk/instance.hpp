#pragma once

#include "../gfx.hpp"
#include "util/std.hpp"

namespace vkn {

struct Instance {
    VkInstance instance;

    VkDebugUtilsMessengerEXT debug_messenger;

    const std::vector<const char*> layers = {
	"VK_LAYER_KHRONOS_validation"
    };

    Instance();
    ~Instance();

    Instance(const Instance &other) = delete;
    Instance(Instance &&other) {
	*this = std::move(other);
    }
    Instance &operator=(const Instance &other) = delete;
    Instance &operator=(Instance &&other) {
	this->instance = other.instance;
	other.instance = VK_NULL_HANDLE;
	return *this;
    }
};

}
