#pragma once

#include <vk_mem_alloc.h>

#include "gfx.hpp"
#include "util/util.hpp"
#include "vk/vk_pipeline.hpp"
#include "vk/vk_command_buffer.hpp"
#include "vk/vk_sync.hpp"
#include "vk/vk_vertex_buffer.hpp"
#include "vk/vk_uniform_buffer.hpp"
#include "vk/vk_file_texture.hpp"
#include "vk/vk_depth_buffer.hpp"

struct Renderer {
    struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
    };

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

    static constexpr u32 FRAMES_IN_FLIGHT = 3;

    VkDescriptorSetLayout descriptor_layout;

    std::unordered_map<
	std::string,
	std::shared_ptr<VKPipeline>> pipelines;

    std::array<
	std::unique_ptr<VKCommandBuffer>,
	FRAMES_IN_FLIGHT> command_buffers;

    std::array<
	std::unique_ptr<VKSemaphore>,
	FRAMES_IN_FLIGHT> image_available_semaphores;

    std::array<
	std::unique_ptr<VKSemaphore>,
	FRAMES_IN_FLIGHT> render_finished_semaphores;

    std::array<
	std::unique_ptr<VKFence>,
	FRAMES_IN_FLIGHT> in_flight_fences;

    u32 current_frame = 0;

    std::unique_ptr<VKDepthBuffer> depth_buffer;

    std::unordered_map<
	std::string,
	std::unique_ptr<VKFileTexture>> textures;

    std::unique_ptr<VKVertexBuffer> vertex_buffer;

    std::array<
	std::unique_ptr<VKUniformBuffer>,
	FRAMES_IN_FLIGHT> uniform_buffers;

    std::array<
	std::unique_ptr<VKUniformBuffer>,
	FRAMES_IN_FLIGHT> brightness_uniform_buffers;

    std::array<VkDescriptorSet, FRAMES_IN_FLIGHT> descriptors;

    Renderer();

    ~Renderer();

    Renderer(const Renderer &other) = delete;
    Renderer(Renderer &&other) = default;
    Renderer &operator=(const Renderer &other) = delete;
    Renderer &operator=(Renderer &&other) = default;

    void render();
};
