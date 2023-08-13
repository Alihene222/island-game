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

std::array<VkVertexInputAttributeDescription, 2>
    vkn::Vertex::get_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 2>
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

    return attribute_descriptions;
}

vkn::VertexBuffer::VertexBuffer(void *data, usize data_size) {
    VkDeviceSize size = data_size;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    vkn::make_buffer(
	size,
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	&staging_buffer,
	&staging_memory);

    void *buf_data;
    vkMapMemory(
	global.vk_global->device->handle,
	staging_memory,
	0,
	size,
	0,
	&buf_data);
    std::memcpy(buf_data, data, size);
    vkUnmapMemory(
	global.vk_global->device->handle,
	staging_memory);

    vkn::make_buffer(
	size,
	VK_BUFFER_USAGE_TRANSFER_DST_BIT
	| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	&this->handle,
	&this->memory);
    vkn::copy_buffer(staging_buffer, this->handle, size);

    vkDestroyBuffer(
	global.vk_global->device->handle,
	staging_buffer,
	nullptr);
    vkFreeMemory(
	global.vk_global->device->handle,
	staging_memory,
	nullptr);
}

vkn::VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(
	global.vk_global->device->handle,
	this->handle,
	nullptr);
    vkFreeMemory(
	global.vk_global->device->handle,
	this->memory,
	nullptr);
}
