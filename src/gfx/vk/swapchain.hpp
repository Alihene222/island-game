#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "pipeline.hpp"

namespace vkn {

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

struct Swapchain {
    VkSwapchainKHR handle;

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;

    std::vector<VkImage> images;

    std::vector<VkImageView> image_views;

    std::vector<VkFramebuffer> framebuffers;

    // Preferred color space
    enum ColorSpace {
	LINEAR,
	SRGB
    } preferred_color_space;

    enum PresentMode {
	IMMEDIATE,
	FIFO,
	MAILBOX
    } preferred_present_mode;

    Swapchain(
	ColorSpace preferred_color_space = SRGB,
	PresentMode preferred_present_mode = MAILBOX);

    ~Swapchain();

    Swapchain(const Swapchain &other) = delete;

    Swapchain(Swapchain &&other) {
	*this = std::move(other);
    }
    Swapchain &operator=(const Swapchain &other) = delete;

    Swapchain &operator=(Swapchain &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	this->surface_format = other.surface_format;
	this->present_mode = other.present_mode;
	this->extent = other.extent;
	this->images = other.images;
	this->image_views = other.image_views;
	this->framebuffers = other.framebuffers;
	return *this;
    }

    void create_framebuffers(VkRenderPass render_pass);

    void adapt(VkRenderPass render_pass);

private:
    void create();

    void destroy();

    SwapchainSupportDetails query_support(
	VkPhysicalDevice device) const;

    void choose_surface_format(
	const std::vector<VkSurfaceFormatKHR> &available_formats);

    void choose_present_mode(
	const std::vector<VkPresentModeKHR> &available_modes);

    void choose_extent(const VkSurfaceCapabilitiesKHR &capabilities);
};

}
