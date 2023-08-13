#pragma once

#include "gfx.hpp"
#include "util/util.hpp"
#include "vk/pipeline.hpp"
#include "vk/command_buffer.hpp"
#include "vk/sync.hpp"

struct Renderer {
    static constexpr u32 layer_count = 1;
    static constexpr std::array<
	const char*,
	layer_count> layers = {
	"VK_LAYER_KHRONOS_validation"
    };

    static constexpr u32 device_extension_count = 1;
    static constexpr std::array<
	const char*,
	device_extension_count> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::unordered_map<
	std::string,
	std::shared_ptr<vkn::Pipeline>> pipelines;

    std::unique_ptr<vkn::CommandPool> command_pool;

    std::unique_ptr<vkn::CommandBuffer> command_buffer;

    std::unique_ptr<vkn::Semaphore> image_available_semaphore;

    std::unique_ptr<vkn::Semaphore> render_finished_semaphore;

    std::unique_ptr<vkn::Fence> in_flight_fence;

    Renderer();

    ~Renderer();

    Renderer(const Renderer &other) = delete;

    Renderer(Renderer &&other) = default;

    Renderer &operator=(const Renderer &other) = delete;

    Renderer &operator=(Renderer &&other) = default;

    void render();
};
