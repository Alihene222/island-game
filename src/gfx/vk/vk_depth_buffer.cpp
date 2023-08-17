#include "vk_depth_buffer.hpp"

#include "global.hpp"

static VkFormat find_supported_format(
    const std::vector<VkFormat> &candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) {
    for(VkFormat format : candidates) {
	VkFormatProperties properties;
	vkGetPhysicalDeviceFormatProperties(
	    global.vk_global->physical_device,
	    format,
	    &properties);

	if(tiling == VK_IMAGE_TILING_LINEAR
	    && (properties.linearTilingFeatures & features)
		== features) {
	    return format;
	} else if(tiling == VK_IMAGE_TILING_OPTIMAL
	    && (properties.optimalTilingFeatures & features)
		== features) {
	    return format;
	}
    }

    log(
	"Failed to find a suitable depth format",
	LOG_LEVEL_FATAL);
    std::exit(-1);
}

VkFormat VKDepthBuffer::find_depth_format() {
    return find_supported_format(
	{
	    VK_FORMAT_D32_SFLOAT,
	    VK_FORMAT_D32_SFLOAT_S8_UINT,
	    VK_FORMAT_D24_UNORM_S8_UINT
	},
	VK_IMAGE_TILING_OPTIMAL,
	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VKDepthBuffer::VKDepthBuffer() {
    VkFormat format = find_depth_format();
    make_image(
	global.vk_global->swapchain->extent.width,
	global.vk_global->swapchain->extent.height,
	format,
	VK_IMAGE_TILING_OPTIMAL,
	VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	&this->image,
	&this->alloc,
	VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

    global.vk_global->allocator->add_entry(
	this->image, this->alloc);

    VkImageViewCreateInfo view_create_info {};
    view_create_info.sType =
	VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_create_info.image = this->image;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask =
	VK_IMAGE_ASPECT_DEPTH_BIT;
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
	    "Failed to create image view for depth buffer",
	    LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

VKDepthBuffer::~VKDepthBuffer() {
    vkDestroyImageView(
	global.vk_global->device->handle,
	this->image_view,
	nullptr);
}
