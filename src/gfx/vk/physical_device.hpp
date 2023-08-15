#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"
#include "instance.hpp"

struct QueueFamilyIndices {
    std::optional<u32> graphics;
    std::optional<u32> present;

    inline bool is_complete() {
	return graphics.has_value() && present.has_value();
    }
};

QueueFamilyIndices find_queue_families(
    VkPhysicalDevice device);

bool check_extensions(VkPhysicalDevice device);

bool is_suitable(VkPhysicalDevice device);

VkPhysicalDevice pick_physical_device();
