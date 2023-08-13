#include "sync.hpp"

#include "global.hpp"

vkn::Semaphore::Semaphore() {
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

vkn::Semaphore::~Semaphore() {
    vkDestroySemaphore(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

vkn::Fence::Fence() {
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

vkn::Fence::~Fence() {
    vkDestroyFence(
	global.vk_global->device->handle,
	this->handle, nullptr);
}

void vkn::Fence::wait() {
    vkWaitForFences(
	global.vk_global->device->handle,
	1, &this->handle, VK_TRUE, UINT64_MAX);
}

void vkn::Fence::reset() {
    vkResetFences(
	global.vk_global->device->handle,
	1, &this->handle);
}
