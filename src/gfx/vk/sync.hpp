#pragma once

#include "gfx/gfx.hpp"
#include "util/util.hpp"

struct VKSemaphore {
    VkSemaphore handle;

    VKSemaphore();

    ~VKSemaphore();

    VKSemaphore(const VKSemaphore &other) = delete;
    VKSemaphore(VKSemaphore &&other) {
	*this = std::move(other);
    }
    VKSemaphore &operator=(const VKSemaphore &other) = delete;
    VKSemaphore &operator=(VKSemaphore &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct VKFence {
    VkFence handle;

    VKFence();

    ~VKFence();

    VKFence(const VKFence &other) = delete;
    VKFence(VKFence &&other) {
	*this = std::move(other);
    }
    VKFence &operator=(const VKFence &other) = delete;
    VKFence &operator=(VKFence &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    void wait();

    void reset();
};
