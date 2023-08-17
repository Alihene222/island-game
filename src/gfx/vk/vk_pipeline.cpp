#include "vk_pipeline.hpp"

#include "util/util.hpp"
#include "global.hpp"
#include "vk_vertex_buffer.hpp"
#include "vk_depth_buffer.hpp"

static std::vector<char> read_shader_file(
    const std::string &path) {
    std::ifstream input(path, std::ios::ate | std::ios::binary);

    if(!input.good()) {
	log(
	    "Failed to open SPIRV file at " + path,
	    LOG_LEVEL_ERROR);
	std::exit(-1);
    }

    u64 file_size = (u64) input.tellg();
    std::vector<char> buffer(file_size);
    input.seekg(0);
    input.read(&buffer[0], file_size);
    input.close();

    return buffer;
}

VKPipeline::VKPipeline(
    std::string vs_path,
    std::string fs_path,
    VkDescriptorSetLayout *descriptor_set_layout) {
    VkAttachmentDescription color_attachment {};
    color_attachment.format =
	global.vk_global->swapchain->surface_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp =
	VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp =
	VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp =
	VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp =
	VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout =
	VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout =
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout =
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format =
	VKDepthBuffer::find_depth_format();
    depth_attachment.loadOp =
	VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp =
	VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp =
	VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp =
	VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.initialLayout =
	VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout =
	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout =
	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask =
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment =
	&depth_attachment_ref;

    std::array<VkAttachmentDescription, 2> attachments = {
	color_attachment,
	depth_attachment
    };

    VkRenderPassCreateInfo render_pass_create_info {};
    render_pass_create_info.sType =
	VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount =
	attachments.size();
    render_pass_create_info.pAttachments = &attachments[0];
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies =
	&dependency;

    if(vkCreateRenderPass(
	global.vk_global->device->handle,
	&render_pass_create_info, nullptr,
	&this->render_pass) != VK_SUCCESS) {
	log("Failed to create render pass", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    const std::vector<char> vs_code = read_shader_file(vs_path);
    const std::vector<char> fs_code = read_shader_file(fs_path);

    VkShaderModule vs_module =
	this->create_shader_module(vs_code);
    VkShaderModule fs_module =
	this->create_shader_module(fs_code);

    VkPipelineShaderStageCreateInfo vs_stage_info {};
    vs_stage_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vs_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vs_stage_info.module = vs_module;
    vs_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo fs_stage_info {};
    fs_stage_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fs_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fs_stage_info.module = fs_module;
    fs_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {
	vs_stage_info,
	fs_stage_info
    };

    std::vector<VkDynamicState> dynamic_states = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info {};
    dynamic_state_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_info.pDynamicStates = &dynamic_states[0];

    auto binding_description =
	Vertex::get_binding_description();
    auto attribute_descriptions =
	Vertex::get_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info {};
    vertex_input_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount =
	attribute_descriptions.size();
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexAttributeDescriptions =
	&attribute_descriptions[0];
    vertex_input_info.pVertexBindingDescriptions =
	&binding_description;

    VkPipelineInputAssemblyStateCreateInfo assembly_info {};
    assembly_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly_info.topology =
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly_info.primitiveRestartEnable = VK_FALSE;
    
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width =
	global.vk_global->swapchain->extent.width;
    viewport.height =
	global.vk_global->swapchain->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = global.vk_global->swapchain->extent;

    VkPipelineViewportStateCreateInfo viewport_info {};
    viewport_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;
    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterization_info {};
    rasterization_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.depthClampEnable = VK_FALSE;
    rasterization_info.polygonMode =
	VK_POLYGON_MODE_FILL;
    rasterization_info.cullMode =
	VK_CULL_MODE_NONE;
    rasterization_info.frontFace =
	VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_info.depthBiasEnable = VK_FALSE;
    rasterization_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampling_state{};
    multisampling_state.sType =
	VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state.sampleShadingEnable = VK_FALSE;
    multisampling_state.rasterizationSamples =
	VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment {};
    color_blend_attachment.colorWriteMask =
	VK_COLOR_COMPONENT_R_BIT
	| VK_COLOR_COMPONENT_G_BIT
	| VK_COLOR_COMPONENT_B_BIT
	| VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo blend_info {};
    blend_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend_info.attachmentCount = 1;
    blend_info.pAttachments = &color_blend_attachment;
    blend_info.logicOpEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info {};
    depth_stencil_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_info.depthTestEnable = VK_TRUE;
    depth_stencil_info.depthWriteEnable = VK_TRUE;
    depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_info.stencilTestEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo layout_create_info {};
    layout_create_info.sType =
	VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = 1;
    layout_create_info.pSetLayouts =
	descriptor_set_layout;

    if(vkCreatePipelineLayout(
	global.vk_global->device->handle,
	&layout_create_info, nullptr,
	&this->layout) != VK_SUCCESS) {
	log("Failed to create pipeline layout", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    VkGraphicsPipelineCreateInfo create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.stageCount = 2;
    create_info.pStages = shader_stages;
    create_info.pVertexInputState = &vertex_input_info;
    create_info.pInputAssemblyState = &assembly_info;
    create_info.pViewportState = &viewport_info;
    create_info.pRasterizationState = &rasterization_info;
    create_info.pMultisampleState = &multisampling_state;
    create_info.pColorBlendState = &blend_info;
    create_info.pDynamicState = &dynamic_state_info;
    create_info.pDepthStencilState = &depth_stencil_info;
    create_info.layout = this->layout;
    create_info.renderPass = this->render_pass;
    create_info.subpass = 0;

    if(vkCreateGraphicsPipelines(
	global.vk_global->device->handle,
	VK_NULL_HANDLE, 1, &create_info,
	nullptr, &this->handle) != VK_SUCCESS) {
	log("Failed to create pipeline", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    vkDestroyShaderModule(
	global.vk_global->device->handle,
	vs_module, nullptr);
    vkDestroyShaderModule(
	global.vk_global->device->handle,
	fs_module, nullptr);
}

VKPipeline::~VKPipeline() {
    vkDestroyPipeline(
	global.vk_global->device->handle,
	this->handle, nullptr);
    vkDestroyPipelineLayout(
	global.vk_global->device->handle,
	this->layout, nullptr);
    vkDestroyRenderPass(
	global.vk_global->device->handle,
	this->render_pass, nullptr);
}

VkShaderModule VKPipeline::create_shader_module(
    const std::vector<char> &code) {
    VkShaderModuleCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const u32*>(&code[0]);

    VkShaderModule shader_module;
    if(vkCreateShaderModule(
	global.vk_global->device->handle,
	&create_info, nullptr, &shader_module) != VK_SUCCESS) {
	log("Failed to create shader module", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    return shader_module;
}
