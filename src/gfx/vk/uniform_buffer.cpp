#include "uniform_buffer.hpp"

#include "buffers.hpp"
#include "global.hpp"

vkn::UniformBuffer::UniformBuffer(usize size) {
    vkn::make_buffer(
	size,
	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
	&this->handle,
	&this->alloc,
	0.0f);
    global.vk_global->allocator->add_entry(
	this->handle, this->alloc);

    vmaMapMemory(
	global.vk_global->allocator->handle,
	this->alloc,
	&this->data);
}

vkn::UniformBuffer::~UniformBuffer() {
    vmaUnmapMemory(
	global.vk_global->allocator->handle,
	this->alloc);	
}

vkn::DescriptorSetLayout::DescriptorSetLayout(
    vkn::DescriptorSetLayout::Stage stage)
    : stage(stage) {
    VkDescriptorSetLayoutBinding layout_binding {};
    layout_binding.binding = 0;
    layout_binding.stageFlags = stage == STAGE_VERTEX ? 
	VK_SHADER_STAGE_VERTEX_BIT
	: VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_binding.descriptorType =
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;

    VkDescriptorSetLayoutCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = 1;
    create_info.pBindings = &layout_binding;

    if(vkCreateDescriptorSetLayout(
	global.vk_global->device->handle,
	&create_info,
	nullptr,
	&this->handle) != VK_SUCCESS) {
	log(
	    "Failed to create descriptor set layout",
	    LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

vkn::DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(
	global.vk_global->device->handle,
	this->handle,
	nullptr);
}

vkn::DescriptorPool::DescriptorPool(u32 frames_in_flight) {
    VkDescriptorPoolSize pool_size {};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount = frames_in_flight;

    VkDescriptorPoolCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = 1;
    create_info.pPoolSizes = &pool_size;
    create_info.maxSets = frames_in_flight;

    if(vkCreateDescriptorPool(
	global.vk_global->device->handle,
	&create_info,
	nullptr,
	&this->handle) != VK_SUCCESS) {
	log("Failed to create descriptor pool", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

vkn::DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(
	global.vk_global->device->handle,
	this->handle,
	nullptr);
}

vkn::DescriptorSet::DescriptorSet(
    const vkn::UniformBuffer &uniform_buffer,
    const vkn::DescriptorSetLayout &layout,
    const vkn::DescriptorPool &pool,
    usize ubo_size) {
    VkDescriptorSetAllocateInfo alloc_info {};
    alloc_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool.handle;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout.handle;

    if(vkAllocateDescriptorSets(
	global.vk_global->device->handle,
	&alloc_info,
	&this->handle) != VK_SUCCESS) {
	log("Failed to allocate descriptor set", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    VkDescriptorBufferInfo buffer_info {};
    buffer_info.buffer = uniform_buffer.handle;
    buffer_info.offset = 0;
    buffer_info.range = ubo_size;

    VkWriteDescriptorSet descriptor_write {};
    descriptor_write.sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = this->handle;
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType =
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(
	global.vk_global->device->handle,
	1,
	&descriptor_write,
	0,
	nullptr);
}
