#pragma once

#include <vk_mem_alloc.h>

#include "gfx.hpp"
#include "util/util.hpp"
#include "vk/pipeline.hpp"
#include "vk/command_buffer.hpp"
#include "vk/sync.hpp"
#include "vk/vertex_buffer.hpp"
#include "vk/uniform_buffer.hpp"
#include "vk/file_texture.hpp"

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

    std::unique_ptr<vkn::DescriptorSetLayout>
	descriptor_set_layout;

    std::unordered_map<
	std::string,
	std::shared_ptr<vkn::Pipeline>> pipelines;

    std::array<
	std::unique_ptr<vkn::CommandBuffer>,
	FRAMES_IN_FLIGHT> command_buffers;

    std::array<
	std::unique_ptr<vkn::Semaphore>,
	FRAMES_IN_FLIGHT> image_available_semaphores;

    std::array<
	std::unique_ptr<vkn::Semaphore>,
	FRAMES_IN_FLIGHT> render_finished_semaphores;

    std::array<
	std::unique_ptr<vkn::Fence>,
	FRAMES_IN_FLIGHT> in_flight_fences;

    u32 current_frame = 0;

    std::unordered_map<
	std::string,
	std::unique_ptr<vkn::FileTexture>> textures;

    std::unique_ptr<vkn::VertexBuffer> vertex_buffer;

    std::array<
	std::unique_ptr<vkn::UniformBuffer>,
	FRAMES_IN_FLIGHT> uniform_buffers;

    std::unique_ptr<vkn::DescriptorPool> descriptor_pool;

    std::array<
	std::unique_ptr<vkn::DescriptorSet>,
	FRAMES_IN_FLIGHT> descriptor_sets;

    Renderer();

    ~Renderer();

    Renderer(const Renderer &other) = delete;
    Renderer(Renderer &&other) = default;
    Renderer &operator=(const Renderer &other) = delete;
    Renderer &operator=(Renderer &&other) = default;

    void render();
};
