#pragma once

#include <glm/glm.hpp>
#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex_coords;

    static VkVertexInputBindingDescription
	get_binding_description(); 

    static std::array<
	VkVertexInputAttributeDescription, 3>
	    get_attribute_descriptions();
};

struct VKVertexBuffer {
    VkBuffer handle;

    VmaAllocation alloc;

    VKVertexBuffer() = default;
    VKVertexBuffer(void *data, usize size);

    ~VKVertexBuffer() = default;

    VKVertexBuffer(const VKVertexBuffer &other) = delete;
    VKVertexBuffer(VKVertexBuffer &&other) {
	*this = std::move(other);
    }
    VKVertexBuffer &operator=(const VKVertexBuffer &other) = delete;
    VKVertexBuffer &operator=(VKVertexBuffer &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	this->alloc = other.alloc;
	other.alloc = VK_NULL_HANDLE;
	return *this;
    }
};
