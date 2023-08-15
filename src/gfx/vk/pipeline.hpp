#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"
#include "uniform_buffer.hpp"

struct VKPipeline {
    VkPipeline handle;

    VkPipelineLayout layout;

    VkRenderPass render_pass;

    VKPipeline() = default;
    VKPipeline(
	std::string vs_path,
	std::string fs_path,
	VkDescriptorSetLayout *descriptor_set_layout);

    ~VKPipeline();

    VKPipeline(const VKPipeline &other) = delete;
    VKPipeline(VKPipeline &&other) {
	*this = std::move(other);
    }
    VKPipeline &operator=(const VKPipeline &other) = delete;
    VKPipeline &operator=(VKPipeline &&other) {
	this->handle = other.handle;
	other.handle = VK_NULL_HANDLE;
	this->layout = other.layout;
	other.layout = VK_NULL_HANDLE;
	this->render_pass = other.render_pass;
	other.render_pass = VK_NULL_HANDLE;
	return *this;
    }

private:
    VkShaderModule create_shader_module(
	const std::vector<char> &code);
};
