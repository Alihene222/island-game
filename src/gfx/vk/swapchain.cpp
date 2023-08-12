#include "swapchain.hpp"

#include "global.hpp"

vkn::Swapchain::Swapchain(
    ColorSpace preferred_color_space,
    PresentMode preferred_present_mode)
    : preferred_color_space(preferred_color_space),
      preferred_present_mode(preferred_present_mode) {
    SwapchainSupportDetails support_details =
	this->query_support(global.vk_global->physical_device);
    this->choose_surface_format(support_details.formats);
    this->choose_present_mode(support_details.present_modes);
    this->choose_extent(support_details.capabilities);

    u32 image_count =
	support_details.capabilities.minImageCount + 1;
    if(support_details.capabilities.maxImageCount > 0
	&& image_count
	    > support_details.capabilities.maxImageCount) {
	image_count = support_details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = global.vk_global->surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = this->surface_format.format;
    create_info.presentMode = this->present_mode;
    create_info.imageExtent = this->extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    vkn::QueueFamilyIndices indices = vkn::find_queue_families(
	global.vk_global->physical_device);
    u32 queue_family_indices[] = {
	indices.graphics.value(),
	indices.present.value()
    };
    if(indices.graphics != indices.present) {
	create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	create_info.queueFamilyIndexCount = 2;
	create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
	create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform =
	support_details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.clipped = true;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(
	global.vk_global->device->handle,
	&create_info, nullptr,
	&this->handle) != VK_SUCCESS) {
	log("Failed to create swapchain", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    vkGetSwapchainImagesKHR(
	global.vk_global->device->handle,
	this->handle, &image_count, nullptr);
    this->images.resize(image_count);
    vkGetSwapchainImagesKHR(
	global.vk_global->device->handle,
	this->handle, &image_count, &this->images[0]);

    this->image_views.resize(this->images.size());
    for(u64 i = 0; i < this->images.size(); i++) {
	VkImageViewCreateInfo image_view_create_info {};
	image_view_create_info.sType =
	    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	image_view_create_info.image = this->images[i];
	image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create_info.format =
	    this->surface_format.format;
	image_view_create_info.components.r =
	    VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_create_info.components.g =
	    VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_create_info.components.b =
	    VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_create_info.components.a =
	    VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_create_info.subresourceRange.aspectMask =
	    VK_IMAGE_ASPECT_COLOR_BIT;
	image_view_create_info.subresourceRange.baseMipLevel = 0;
	image_view_create_info.subresourceRange.levelCount = 1;
	image_view_create_info.subresourceRange.baseArrayLayer = 0;
	image_view_create_info.subresourceRange.layerCount = 1;

	if(vkCreateImageView(
	    global.vk_global->device->handle,
	    &image_view_create_info, nullptr,
	    &this->image_views[i]) != VK_SUCCESS) {
	    log("Failed to create image views", LOG_LEVEL_FATAL);
	    std::exit(-1);
	}
    }
}

vkn::Swapchain::~Swapchain() {
    for(const auto &image_view : this->image_views) {
	vkDestroyImageView(
	    global.vk_global->device->handle,
	    image_view, nullptr);
    }
    vkDestroySwapchainKHR(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

vkn::SwapchainSupportDetails vkn::Swapchain::query_support(
    VkPhysicalDevice device) const {
    vkn::SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
	device, global.vk_global->surface,
	&details.capabilities);

    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
	device, global.vk_global->surface,
	&format_count, nullptr);

    if(format_count > 0) {
	details.formats.resize(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(
	    device, global.vk_global->surface,
	    &format_count, &details.formats[0]);
    }

    u32 present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
	device, global.vk_global->surface,
	&present_mode_count, nullptr);

    if(present_mode_count > 0) {
	details.present_modes.resize(present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(
	    device, global.vk_global->surface,
	    &present_mode_count,
	    &details.present_modes[0]);
    }

    return details;
}

void vkn::Swapchain::choose_surface_format(
    const std::vector<VkSurfaceFormatKHR> &available_formats) {
    bool selected = false;

    for(const auto &format : available_formats) {
	if(this->preferred_color_space == LINEAR) {
	    if(format.format == VK_FORMAT_B8G8R8A8_UNORM
		&& format.colorSpace
		    == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		this->surface_format = format;
		selected = true;
	    }
	} else {
	    if(format.format == VK_FORMAT_B8G8R8A8_SRGB
		&& format.colorSpace
		    == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		this->surface_format = format;
		selected = true;
	    }
	}
    }

    if(!selected) {
	this->surface_format = available_formats[0];
    }
}

void vkn::Swapchain::choose_present_mode(
    const std::vector<VkPresentModeKHR> &available_modes) {
    bool selected = false;

    for(const auto &mode : available_modes) {
	switch(this->preferred_present_mode) {
	    case IMMEDIATE:
		if(mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		    this->present_mode = mode;
		    selected = true;
		}
		break;
	    case FIFO:
		if(mode == VK_PRESENT_MODE_FIFO_KHR) {
		    this->present_mode = mode;
		    selected = true;
		}
		break;
	    case MAILBOX:
		if(mode == VK_PRESENT_MODE_MAILBOX_KHR) {
		    this->present_mode = mode;
		    selected = true;
		}
		break;
	}
    }

    if(!selected) {
	this->present_mode = available_modes[0];
    }
}

void vkn::Swapchain::choose_extent(
    const VkSurfaceCapabilitiesKHR &capabilities) {
    if(capabilities.currentExtent.width
	!= std::numeric_limits<u32>::max()) {
	this->extent = capabilities.currentExtent;
    } else {
	std::tuple tuple = global.platform->window->get_size();
	u32 width = static_cast<u32>(std::get<0>(tuple));
	u32 height = static_cast<u32>(std::get<1>(tuple));

	VkExtent2D actual_extent = {
	    width,
	    height
	};

	actual_extent.width = std::clamp(
	    actual_extent.width,
	    capabilities.minImageExtent.width,
	    capabilities.maxImageExtent.width);
	actual_extent.height = std::clamp(
	    actual_extent.height,
	    capabilities.minImageExtent.height,
	    capabilities.maxImageExtent.height);

	this->extent = actual_extent;
    }
}
