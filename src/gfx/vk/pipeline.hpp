#pragma once

#include "gfx/gfx.hpp"
#include "util/std.hpp"

namespace vkn {

struct Pipeline {
    VkPipeline handle;

    VkPipelineLayout layout;

    VkRenderPass render_pass;

    Pipeline(std::string vs_path, std::string fs_path);

    ~Pipeline();

    Pipeline(const Pipeline &other) = delete;

    Pipeline(Pipeline &&other) {
	*this = std::move(other);
    }

    Pipeline &operator=(const Pipeline &other) = delete;

    Pipeline &operator=(Pipeline &&other) {
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

}
