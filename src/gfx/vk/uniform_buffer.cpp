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

    VkDescriptorSetLayoutBinding sampler_binding {};
    sampler_binding.binding = 1;
    sampler_binding.stageFlags =
	VK_SHADER_STAGE_FRAGMENT_BIT;
    sampler_binding.descriptorType =
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_binding.descriptorCount = 1;

    VkDescriptorSetLayoutBinding bindings[] = {
	layout_binding,
	sampler_binding
    };

    VkDescriptorSetLayoutCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = 2;
    create_info.pBindings = bindings;

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
    std::array<VkDescriptorPoolSize, 2> pool_sizes {};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = frames_in_flight;
    pool_sizes[1].type =
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = frames_in_flight;

    VkDescriptorPoolCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = pool_sizes.size();
    create_info.pPoolSizes = &pool_sizes[0];
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
    usize ubo_size,
    const FileTexture &texture) {
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

    VkDescriptorImageInfo image_info {};
    image_info.imageLayout =
	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture.image_view;
    image_info.sampler = texture.sampler;

    std::array<VkWriteDescriptorSet, 2> descriptor_writes {};

    descriptor_writes[0].sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = this->handle;
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType =
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    descriptor_writes[1].sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = this->handle;
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType =
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(
	global.vk_global->device->handle,
	descriptor_writes.size(),
	&descriptor_writes[0],
	0,
	nullptr);
}
