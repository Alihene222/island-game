#include "uniform_buffer.hpp"

#include "buffers.hpp"
#include "global.hpp"

vkn::UniformBuffer::UniformBuffer(usize size) : size(size) {
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

VkDescriptorBufferInfo vkn::UniformBuffer::descriptor_info() {
    VkDescriptorBufferInfo buffer_info {};
    buffer_info.buffer = this->handle;
    buffer_info.offset = 0;
    buffer_info.range = this->size;
    return buffer_info;
}

vkn::DescriptorSetLayout::DescriptorSetLayout(
    const std::vector<Binding> &bindings) {
    
    std::vector<VkDescriptorSetLayoutBinding> vk_bindings;

    for(const auto &binding : bindings) {
	VkDescriptorSetLayoutBinding vk_binding {};
	vk_binding.binding = binding.location;
	vk_binding.stageFlags = binding.stage == STAGE_VERTEX ?
	    VK_SHADER_STAGE_VERTEX_BIT
	    : VK_SHADER_STAGE_FRAGMENT_BIT;
	vk_binding.descriptorType = binding.type;
	vk_binding.descriptorCount = 1;

	vk_bindings.push_back(vk_binding);
    }

    VkDescriptorSetLayoutCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = bindings.size();
    create_info.pBindings = &vk_bindings[0];

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
    const vkn::DescriptorSetLayout &layout,
    const vkn::DescriptorPool &pool) {
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
}

vkn::DescriptorSet &vkn::DescriptorSet::add_uniform(
    VkDescriptorBufferInfo *info) {
    VkWriteDescriptorSet descriptor_write {};
    descriptor_write.sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = this->handle;
    descriptor_write.dstBinding = this->info.size;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType =
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = info;

    this->info.descriptor_writes.push_back(descriptor_write);
    this->info.size++;

    return *this;
}

vkn::DescriptorSet &vkn::DescriptorSet::add_sampler(
    VkDescriptorImageInfo *info) {
    VkWriteDescriptorSet descriptor_write {};
    descriptor_write.sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = this->handle;
    descriptor_write.dstBinding = this->info.size;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType =
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pImageInfo = info;

    this->info.descriptor_writes.push_back(descriptor_write);
    this->info.size++;

    return *this;
}

void vkn::DescriptorSet::submit() {
    vkUpdateDescriptorSets(
	global.vk_global->device->handle,
	this->info.descriptor_writes.size(),
	&this->info.descriptor_writes[0],
	0,
	nullptr);

    this->info.clear();
}
