#include "vertex_buffer.hpp"

#include "buffers.hpp"
#include "global.hpp"

VkVertexInputBindingDescription
    vkn::Vertex::get_binding_description() {
    VkVertexInputBindingDescription binding_description;
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate =
	VK_VERTEX_INPUT_RATE_VERTEX;
    return binding_description;
}

std::array<VkVertexInputAttributeDescription, 3>
    vkn::Vertex::get_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 3>
	attribute_descriptions {};

    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format =
	VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset =
	offsetof(Vertex, pos);

    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format =
	VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset =
	offsetof(Vertex, color);

    attribute_descriptions[2].binding = 0;
    attribute_descriptions[2].location = 2;
    attribute_descriptions[2].format =
	VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[2].offset =
	offsetof(Vertex, tex_coords);

    return attribute_descriptions;
}

vkn::VertexBuffer::VertexBuffer(void *data, usize size) {
    VkBuffer staging_buffer;
    VmaAllocation staging_alloc;

    vkn::make_buffer(
	size,
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
	&staging_buffer,
	&staging_alloc,
	0.0f);

    void *buf_data;
    vmaMapMemory(
	global.vk_global->allocator->handle,
	staging_alloc,
	&buf_data);
    std::memcpy(buf_data, data, size);
    vmaUnmapMemory(
	global.vk_global->allocator->handle,
	staging_alloc);

    vkn::make_buffer(
	size,
	VK_BUFFER_USAGE_TRANSFER_DST_BIT
	| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
	&this->handle,
	&this->alloc,
	1.0f);
    global.vk_global->allocator->add_entry(
	this->handle, this->alloc);
    vkn::copy_buffer(staging_buffer, this->handle, size);

    vmaDestroyBuffer(
	global.vk_global->allocator->handle,
	staging_buffer,
	staging_alloc);
}
