#pragma once

#include <glm/glm.hpp>
#include <vk_mem_alloc.h>

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "util/util.hpp"

namespace vkn {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription
	get_binding_description(); 

    static std::array<
	VkVertexInputAttributeDescription, 2>
	    get_attribute_descriptions();
};

struct VertexBuffer {
    VkBuffer handle;

    VmaAllocation alloc;

    VertexBuffer() = default;
    VertexBuffer(void *data, usize size);

    ~VertexBuffer() = default;

    VertexBuffer(const VertexBuffer &other) = delete;
    VertexBuffer(VertexBuffer &&other) {
	*this = std::move(other);
    }
    VertexBuffer &operator=(const VertexBuffer &other) = delete;
    VertexBuffer &operator=(VertexBuffer &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	this->alloc = other.alloc;
	other.alloc = VK_NULL_HANDLE;
	return *this;
    }
};

}
