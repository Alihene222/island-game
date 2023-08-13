#pragma once

#include "gfx/gfx.hpp"
#include "util/util.hpp"

namespace vkn {

struct Semaphore {
    VkSemaphore handle;

    Semaphore();

    ~Semaphore();

    Semaphore(const Semaphore &other) = delete;
    Semaphore(Semaphore &&other) {
	*this = std::move(other);
    }
    Semaphore &operator=(const Semaphore &other) = delete;
    Semaphore &operator=(Semaphore &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }
};

struct Fence {
    VkFence handle;

    Fence();

    ~Fence();

    Fence(const Fence &other) = delete;
    Fence(Fence &&other) {
	*this = std::move(other);
    }
    Fence &operator=(const Fence &other) = delete;
    Fence &operator=(Fence &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	return *this;
    }

    void wait();

    void reset();
};

}
