#pragma once

#include "gfx/gfx.hpp"
#include "util/util.hpp"

namespace vkn {

u32 find_memory_type(
    u32 type_filter,
    VkMemoryPropertyFlags properties);

void make_buffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer *buffer,
    VkDeviceMemory *memory);

void copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

}
