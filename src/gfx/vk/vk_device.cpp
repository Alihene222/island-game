#include "vk_device.hpp"

#include "vk_physical_device.hpp"
#include "gfx/renderer.hpp"
#include "global.hpp"

VKDevice::VKDevice() {
    QueueFamilyIndices indices =
	find_queue_families(
	    global.vk_global->physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<u32> unique_queue_families =
	{ indices.graphics.value(), indices.present.value() };

    f32 queue_priority = 1.0f;
    for(UNUSED u32 queue_family : unique_queue_families) {
	VkDeviceQueueCreateInfo queue_create_info {};
	queue_create_info.sType =
	    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex =
	    indices.graphics.value();
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &queue_priority;
	queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_infos[0];
    create_info.queueCreateInfoCount =
	static_cast<u32>(queue_create_infos.size());
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount =
	Renderer::device_extension_count;
    create_info.ppEnabledExtensionNames =
	&Renderer::device_extensions[0];

    if(global.debug) {
	create_info.enabledLayerCount = Renderer::layer_count;
	create_info.ppEnabledLayerNames = &Renderer::layers[0];
    } else {
	create_info.enabledLayerCount = 0;
    }

    if(vkCreateDevice(
	global.vk_global->physical_device,
	&create_info,
	nullptr, &this->handle) != VK_SUCCESS) {
	log("Failed to create device", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    vkGetDeviceQueue(
	this->handle,
	indices.graphics.value(),
	0, &this->queue_graphics);
    vkGetDeviceQueue(
	this->handle,
	indices.present.value(),
	0, &this->queue_present);
}

VKDevice::~VKDevice() {
    vkDestroyDevice(this->handle, nullptr);
}

void VKDevice::wait_idle() {
    vkDeviceWaitIdle(this->handle);
}
