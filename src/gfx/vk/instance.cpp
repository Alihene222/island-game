#include "instance.hpp"

#include "global.hpp"
#include "gfx/renderer.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    UNUSED VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    UNUSED VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    UNUSED void* user_data) {
    log(
	"Vulkan: "
	+ std::string(callback_data->pMessage),
	LOG_LEVEL_DEBUG);
    return VK_FALSE;
}

static VkResult vk_create_debug_utils_messenger_EXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func =
	(PFN_vkCreateDebugUtilsMessengerEXT)
	    vkGetInstanceProcAddr(
		instance,
		"vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(
	    instance,
	    pCreateInfo,
	    pAllocator,
	    pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void vk_destroy_debug_utils_messenger_EXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func =
	(PFN_vkDestroyDebugUtilsMessengerEXT)
	    vkGetInstanceProcAddr(
		instance,
		"vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static auto make_debug_messenger_create_info() {
    VkDebugUtilsMessengerCreateInfoEXT create_info {};
    create_info.sType =
	VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity =
	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
	| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
	| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType =
	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
	| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
	| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    return create_info;
}

VKInstance::VKInstance() {
    VkApplicationInfo app_info {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    std::tuple<u32, const char**> tuple =
	global.platform->window->get_extension_info();

    auto glfw_extension_count = std::get<0>(tuple);
    auto glfw_extensions = std::get<1>(tuple);
    std::vector<const char*> extensions(
	glfw_extensions, glfw_extensions + glfw_extension_count);
    if(global.debug) {
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    create_info.enabledExtensionCount =
	static_cast<u32>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    // For instance creation
    VkDebugUtilsMessengerCreateInfoEXT dbg_create_info_pnext;
    if(global.debug) {
	create_info.enabledLayerCount = Renderer::layer_count;
	create_info.ppEnabledLayerNames = &Renderer::layers[0];

	dbg_create_info_pnext = make_debug_messenger_create_info();
	create_info.pNext = &dbg_create_info_pnext;
    } else {
	create_info.enabledLayerCount = 0;
	create_info.pNext = nullptr;
    }

    if(vkCreateInstance(
	&create_info,
	nullptr, &this->handle) != VK_SUCCESS) {
	log("Failed to create vulkan instance", LOG_LEVEL_FATAL);
	std::exit(-1);
    }

    if(global.debug) {
	auto dbg_create_info = make_debug_messenger_create_info();
	if(vk_create_debug_utils_messenger_EXT(
	    this->handle,
	    &dbg_create_info,
	    nullptr,
	    &this->debug_messenger) != VK_SUCCESS) {
	    log(
		"Failed to create debug messenger",
		LOG_LEVEL_FATAL);
	    std::exit(-1);
	}
    }
}

VKInstance::~VKInstance() {
    if(global.debug) {
	vk_destroy_debug_utils_messenger_EXT(
	    this->handle, this->debug_messenger, nullptr);
    }

    vkDestroyInstance(this->handle, nullptr);
}
