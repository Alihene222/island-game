#pragma once

#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/util.hpp"

void make_buffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    i32 flags,
    VkBuffer *buffer,
    VmaAllocation *alloc,
    f32 priority);

void copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
