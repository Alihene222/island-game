#include "file_texture.hpp"

#include <stb/stb_image.h>

#include "global.hpp"

static void transition_layout(
    VkImage image,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout) {
    vkn::CommandBuffer command_buffer =
	vkn::cmd_begin_single();

    VkImageMemoryBarrier barrier {};
    barrier.sType =
	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask =
	VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;

    if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED
	&& new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	&& new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
	log(
	    "Attempted to carry out an unsupported layout tranisition",
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    vkCmdPipelineBarrier(
	command_buffer.handle,
	src_stage,
	dst_stage,
	0,
	0, nullptr,
	0, nullptr,
	1, &barrier);

    vkn::cmd_end_single(command_buffer);
}

static void copy_buffer_to_image(
    VkBuffer buffer,
    VkImage image,
    u32 width,
    u32 height) {
    vkn::CommandBuffer command_buffer =
	vkn::cmd_begin_single();

    VkBufferImageCopy copy_region {};
    copy_region.bufferOffset = 0;
    copy_region.bufferRowLength = 0;
    copy_region.bufferImageHeight = 0;

    copy_region.imageSubresource.aspectMask =
	VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.imageSubresource.mipLevel = 0;
    copy_region.imageSubresource.baseArrayLayer = 0;
    copy_region.imageSubresource.layerCount = 1;

    copy_region.imageOffset = { 0, 0, 0 };
    copy_region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(
	command_buffer.handle,
	buffer, image,
	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	1,
	&copy_region);

    vkn::cmd_end_single(command_buffer);
}

vkn::FileTexture::FileTexture(std::string path) {
    i32 tex_width, tex_height, tex_channels;
    stbi_uc *data = stbi_load(
	path.c_str(),
	&tex_width,
	&tex_height,
	&tex_channels,
	STBI_rgb_alpha);

    if(!data) {
	log(
	    "Failed to load texture at path " + path,
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    VkDeviceSize size = tex_width * tex_height * 4;

    VkBuffer staging_buffer;
    VmaAllocation staging_alloc;
    vkn::make_buffer(
	size,
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
	&staging_buffer,
	&staging_alloc,
	1.0f);

    void *buf_data;
    vmaMapMemory(
	global.vk_global->allocator->handle,
	staging_alloc,
	&buf_data);
    std::memcpy(buf_data, data, size);
    vmaUnmapMemory(
	global.vk_global->allocator->handle,
	staging_alloc);

    stbi_image_free(data);

    VkImageCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.extent.width = tex_width;
    create_info.extent.height = tex_height;
    create_info.extent.depth = 1;
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage =
	VK_IMAGE_USAGE_TRANSFER_DST_BIT
	| VK_IMAGE_USAGE_SAMPLED_BIT;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo alloc_create_info {};
    alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if(vmaCreateImage(
	global.vk_global->allocator->handle,
	&create_info,
	&alloc_create_info,
	&this->image,
	&this->image_alloc,
	nullptr) != VK_SUCCESS) {
	log(
	    "Failed to create image for path " + path,
	    LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    global.vk_global->allocator->add_entry(
	this->image,
	this->image_alloc);

    transition_layout(
	this->image,
	VK_FORMAT_R8G8B8A8_SRGB,
	VK_IMAGE_LAYOUT_UNDEFINED,
	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copy_buffer_to_image(
	staging_buffer,
	this->image,
	tex_width,
	tex_height);
    transition_layout(
	this->image,
	VK_FORMAT_R8G8B8A8_SRGB,
	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vmaDestroyBuffer(
	global.vk_global->allocator->handle,
	staging_buffer,
	staging_alloc);

    VkImageViewCreateInfo view_create_info {};
    view_create_info.sType =
	VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_create_info.image = this->image;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    view_create_info.subresourceRange.aspectMask =
	VK_IMAGE_ASPECT_COLOR_BIT;
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    if(vkCreateImageView(
	global.vk_global->device->handle,
	&view_create_info,
	nullptr,
	&this->image_view) != VK_SUCCESS) {
	log(
	    "Failed to create image view for " + path,
	    LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    VkSamplerCreateInfo sampler_create_info {};
    sampler_create_info.sType =
	VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.minFilter = VK_FILTER_NEAREST;
    sampler_create_info.magFilter = VK_FILTER_NEAREST;
    sampler_create_info.addressModeU =
	VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV =
	VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW =
	VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties device_properties {};
    vkGetPhysicalDeviceProperties(
	global.vk_global->physical_device,
	&device_properties);
    sampler_create_info.anisotropyEnable = VK_TRUE;
    sampler_create_info.maxAnisotropy =
	device_properties.limits.maxSamplerAnisotropy;

    sampler_create_info.borderColor =
	VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = VK_FALSE;
    sampler_create_info.compareEnable = VK_FALSE;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode =
	VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.maxLod = 0.0f;
    sampler_create_info.minLod = 0.0f;

    if(vkCreateSampler(
	global.vk_global->device->handle,
	&sampler_create_info,
	nullptr,
	&this->sampler) != VK_SUCCESS) {
	log(
	    "Failed to create image sampler for " + path,
	    LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

vkn::FileTexture::~FileTexture() {
    vkDestroySampler(
	global.vk_global->device->handle,
	this->sampler,
	nullptr);
    vkDestroyImageView(
	global.vk_global->device->handle,
	this->image_view,
	nullptr);
}

VkDescriptorImageInfo vkn::FileTexture::descriptor_info() {
    VkDescriptorImageInfo image_info {};
    image_info.imageLayout =
	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = this->image_view;
    image_info.sampler = this->sampler;
    return image_info;
}
