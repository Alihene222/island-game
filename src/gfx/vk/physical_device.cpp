#include "physical_device.hpp"

#include "gfx/renderer.hpp"
#include "global.hpp"

QueueFamilyIndices find_queue_families(
    VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
	device,
	&queue_family_count,
	nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(
	queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
	device,
	&queue_family_count,
	queue_families.data());

    u32 i = 0;
    for(const auto &queue_family : queue_families) {
	VkBool32 present_support = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(
	    device, i,
	    global.vk_global->surface,
	    &present_support);

	if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
	    indices.graphics = i;
	}

	if(present_support) {
	    indices.present = i;
	}

	if(indices.is_complete()) {
	    break;
	}

	i++;
    }

    return indices;
}

bool check_extensions(VkPhysicalDevice device) {
    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties(
	device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(
	extension_count);
    vkEnumerateDeviceExtensionProperties(
	device,
	nullptr,
	&extension_count,
	&available_extensions[0]);

    std::set<std::string> needed_extensions(
	std::begin(Renderer::device_extensions),
	std::end(Renderer::device_extensions));

    for(const auto &extension : available_extensions) {
	needed_extensions.erase(extension.extensionName);
    }

    return needed_extensions.empty();
}

bool is_suitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = find_queue_families(device);

    bool extensions_supported = check_extensions(device);

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(
	device, &supported_features);

    return indices.is_complete() 
	&& extensions_supported
	&& supported_features.samplerAnisotropy;
}

VkPhysicalDevice pick_physical_device() {
    u32 count = 0;
    vkEnumeratePhysicalDevices(
	global.vk_global->instance->handle,
	&count, nullptr);

    if(count == 0) {
	log(
	    "Failed to find a GPU with vulkan support",
	    LOG_LEVEL_FATAL);
	// TODO: return a result instead of exiting immediately
	return VK_NULL_HANDLE;
    }

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(
	global.vk_global->instance->handle,
	&count, &devices[0]);
    
    for(const auto &device : devices) {
	if(is_suitable(device)) {
	    return device;
	}
    }

    log("Failed to find a suitable GPU", LOG_LEVEL_FATAL);
    return VK_NULL_HANDLE;
}
