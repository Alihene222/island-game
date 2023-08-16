#include "vk_uniform_buffer.hpp"

#include "vk_buffers.hpp"
#include "global.hpp"

static VkDescriptorPool create_pool(
    VKDescriptorAllocator::PoolSizes &pool_sizes,
    u32 count,
    VkDescriptorPoolCreateFlags flags) {
    std::vector<VkDescriptorPoolSize> sizes;
    for(const auto &[type, size] : pool_sizes.sizes) {
	sizes.push_back({ type, u32(size * count) });
    }
    VkDescriptorPoolCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.flags = flags;
    create_info.maxSets = count;
    create_info.poolSizeCount = sizes.size();
    create_info.pPoolSizes = &sizes[0];

    VkDescriptorPool pool;
    if(vkCreateDescriptorPool(
	global.vk_global->device->handle,
	&create_info,
	nullptr,
	&pool) != VK_SUCCESS) {
	log("Failed to create descriptor pool", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    return pool;
}

VKUniformBuffer::VKUniformBuffer(usize size) : size(size) {
    make_buffer(
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

VKUniformBuffer::~VKUniformBuffer() {
    vmaUnmapMemory(
	global.vk_global->allocator->handle,
	this->alloc);	
}

VkDescriptorBufferInfo VKUniformBuffer::descriptor_info() {
    VkDescriptorBufferInfo buffer_info {};
    buffer_info.buffer = this->handle;
    buffer_info.offset = 0;
    buffer_info.range = this->size;
    return buffer_info;
}

VkDescriptorSet VKDescriptorAllocator::alloc(
    VkDescriptorSetLayout &layout) {
    VkDescriptorSet set;

    if(this->current_pool == VK_NULL_HANDLE) {
	this->current_pool = this->get_pool();
	this->used_pools.push_back(this->current_pool);
    }

    VkDescriptorSetAllocateInfo alloc_info {};
    alloc_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.pSetLayouts = &layout;
    alloc_info.descriptorPool = current_pool;
    alloc_info.descriptorSetCount = 1;

    VkResult result = vkAllocateDescriptorSets(
	global.vk_global->device->handle,
	&alloc_info,
	&set);

    bool need_reallocate = false;

    switch(result) {
	case VK_SUCCESS:
	    return set;
	    break;
	case VK_ERROR_FRAGMENTED_POOL:
	case VK_ERROR_OUT_OF_POOL_MEMORY:
	    need_reallocate = true;
	    break;
	default:
	    log(
		"Failed to allocate descriptor set",
		LOG_LEVEL_FATAL);
	    std::exit(-1);
	    break;
    }
    
    if(need_reallocate) {
	this->current_pool = get_pool();
	this->used_pools.push_back(this->current_pool);
	alloc_info.descriptorPool = this->current_pool;
	
	if(vkAllocateDescriptorSets(
	    global.vk_global->device->handle,
	    &alloc_info,
	    &set) != VK_SUCCESS) {
	    log(
		"Failed to allocate descriptor set",
		LOG_LEVEL_FATAL);
	    std::exit(-1);
	}
    }

    return set;
}

void VKDescriptorAllocator::reset() {
    for(auto p : this->used_pools) {
	vkResetDescriptorPool(
	    global.vk_global->device->handle, p, 0);
	this->free_pools.push_back(p);
    }

    this->used_pools.clear();

    this->current_pool = VK_NULL_HANDLE;
}

void VKDescriptorAllocator::cleanup() {
    for(auto pool : this->used_pools) {
	vkDestroyDescriptorPool(
	    global.vk_global->device->handle,
	    pool,
	    nullptr);
    }

    for(auto pool : this->free_pools) {
	vkDestroyDescriptorPool(
	    global.vk_global->device->handle,
	    pool,
	    nullptr);
    }
}

VkDescriptorPool VKDescriptorAllocator::get_pool() {
    if(this->free_pools.size() > 0) {
	VkDescriptorPool pool = this->free_pools.back();
	free_pools.pop_back();
	return pool;
    } else {
	return create_pool(this->descriptor_sizes, 1000, 0);
    }
}

VKDescriptorLayoutCache::~VKDescriptorLayoutCache() {
    for(auto pair : this->layout_cache) {
	vkDestroyDescriptorSetLayout(
	    global.vk_global->device->handle,
	    pair.second,
	    nullptr);
    }
}

VkDescriptorSetLayout VKDescriptorLayoutCache::create_layout(
    VkDescriptorSetLayoutCreateInfo *create_info) {
    DescriptorLayoutInfo layout_info;
    layout_info.bindings.reserve(
	create_info->bindingCount);

    bool is_sorted = true;
    u32 last_binding = 0;

    for(u32 i = 0; i < create_info->bindingCount; i++) {
	layout_info.bindings.push_back(
	    create_info->pBindings[i]);

	if(create_info->pBindings[i].binding > last_binding) {
	    last_binding = create_info->pBindings[i].binding;
	} else {
	    is_sorted = false;
	}
    }

    if(!is_sorted) {
	std::sort(
	    layout_info.bindings.begin(),
	    layout_info.bindings.end(),
	    [](
		VkDescriptorSetLayoutBinding &a,
		VkDescriptorSetLayoutBinding &b){
		    return a.binding < b.binding;
		});
    }

    auto it = this->layout_cache.find(layout_info);
    if(it != this->layout_cache.end()) {
	return (*it).second;
    } else {
	VkDescriptorSetLayout layout;

	if(vkCreateDescriptorSetLayout(
	    global.vk_global->device->handle,
	    create_info,
	    nullptr,
	    &layout) != VK_SUCCESS) {
	    log(
		"Failed to create descriptor layout",
		LOG_LEVEL_FATAL);
	    std::exit(-1);
	}

	this->layout_cache[layout_info] = layout;
	return layout;
    }
}

bool VKDescriptorLayoutCache
    ::DescriptorLayoutInfo
    ::operator==(
    const DescriptorLayoutInfo &other) const {
    if (other.bindings.size() != this->bindings.size()){
	return false;
    } else {
	for (u32 i = 0; i < this->bindings.size(); i++) {
	    if (other.bindings[i].binding
		!= this->bindings[i].binding){
		    return false;
	    }
	    if (other.bindings[i].descriptorType
		!= this->bindings[i].descriptorType){
		    return false;
	    }
	    if (other.bindings[i].descriptorCount
		!= this->bindings[i].descriptorCount){
		    return false;
	    }
	    if (other.bindings[i].stageFlags
		!= this->bindings[i].stageFlags){
		    return false;
	    }
	}
	return true;
    }
}

usize VKDescriptorLayoutCache
    ::DescriptorLayoutInfo
    ::hash() const {

    usize result = std::hash<usize>()(this->bindings.size());

    for(const auto &binding : this->bindings) {
	usize binding_hash =
	    binding.binding
	    | binding.descriptorType << 8
	    | binding.descriptorCount << 16
	    | binding.stageFlags << 24;

	result ^= std::hash<usize>()(binding_hash);
    }

    return result;
}

VKDescriptorBuilder VKDescriptorBuilder::begin(
    VKDescriptorAllocator *allocator,
    VKDescriptorLayoutCache *cache) {
    VKDescriptorBuilder builder;

    builder.allocator = allocator;
    builder.cache = cache;
    return builder;
}

VKDescriptorBuilder &VKDescriptorBuilder::bind_buffer(
    u32 binding,
    VkDescriptorBufferInfo *info,
    VkDescriptorType type,
    VkShaderStageFlags stage) {
    VkDescriptorSetLayoutBinding new_binding {};
    new_binding.binding = binding;
    new_binding.descriptorType = type;
    new_binding.descriptorCount = 1;
    new_binding.pImmutableSamplers = nullptr;
    new_binding.stageFlags = stage;

    this->bindings.push_back(new_binding);

    VkWriteDescriptorSet new_write {};
    new_write.sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    new_write.pNext = nullptr;
    new_write.descriptorCount = 1;
    new_write.pBufferInfo = info;
    new_write.dstBinding = binding;
    new_write.descriptorType = type;

    this->writes.push_back(new_write);
    return *this;
}

VKDescriptorBuilder &VKDescriptorBuilder::bind_image(
    u32 binding,
    VkDescriptorImageInfo *info,
    VkDescriptorType type,
    VkShaderStageFlags stage) {
    VkDescriptorSetLayoutBinding new_binding {};
    new_binding.binding = binding;
    new_binding.descriptorType = type;
    new_binding.descriptorCount = 1;
    new_binding.pImmutableSamplers = nullptr;
    new_binding.stageFlags = stage;

    this->bindings.push_back(new_binding);

    VkWriteDescriptorSet new_write {};
    new_write.sType =
	VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    new_write.pNext = nullptr;
    new_write.descriptorCount = 1;
    new_write.pImageInfo = info;
    new_write.dstBinding = binding;
    new_write.descriptorType = type;

    this->writes.push_back(new_write);
    return *this;
}

void VKDescriptorBuilder::build(
    VkDescriptorSet &set,
    VkDescriptorSetLayout &layout) {
    VkDescriptorSetLayoutCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.pNext = nullptr;

    create_info.bindingCount = this->bindings.size();
    create_info.pBindings = &this->bindings[0];

    layout = this->cache->create_layout(&create_info);

    set = this->allocator->alloc(layout);

    for(auto &w : this->writes) {
	w.dstSet = set;
    }

    vkUpdateDescriptorSets(
	global.vk_global->device->handle,
	this->writes.size(),
	&this->writes[0],
	0,
	nullptr);
}
