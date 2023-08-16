#include "vk_sync.hpp"

#include "global.hpp"

VKSemaphore::VKSemaphore() {
    VkSemaphoreCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if(vkCreateSemaphore(
	global.vk_global->device->handle,
	&create_info, nullptr,
	&this->handle) != VK_SUCCESS) {
	log("Failed to create semaphore", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

VKSemaphore::~VKSemaphore() {
    vkDestroySemaphore(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

VKFence::VKFence() {
    VkFenceCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.flags =
	VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateFence(
	global.vk_global->device->handle,
	&create_info, nullptr,
	&this->handle) != VK_SUCCESS) {
	log("Failed to create fence", LOG_LEVEL_FATAL);
	std::exit(-1);
    }
}

VKFence::~VKFence() {
    vkDestroyFence(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

void VKFence::wait() {
    vkWaitForFences(
	global.vk_global->device->handle,
	1, &this->handle, VK_TRUE, UINT64_MAX);
}

void VKFence::reset() {
    vkResetFences(
	global.vk_global->device->handle,
	1, &this->handle);
}
