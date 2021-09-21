//
// Created by jakob on 20.09.21.
//

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <SDL_vulkan.h>
#include "eos/core/graphics/Graphics.hpp"

eos::GraphicsVulkan::~GraphicsVulkan() {

}

void eos::GraphicsVulkan::setup(std::string_view title) {
	app_ = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = title.data(),
			.applicationVersion = 0, // FIXME: use actual version here
			.pEngineName = "eos",
			.engineVersion = 0, // FIXME: use actual version here
			.apiVersion = VK_API_VERSION_1_0
	};
}

void eos::GraphicsVulkan::instantiate(std::experimental::observer_ptr<SDL_Window> window) {
	window_ = window;

	unsigned int count;
	if(!SDL_Vulkan_GetInstanceExtensions(window_.get(), &count, nullptr))
		SPDLOG_CRITICAL("Failed to get the required extensions");

	std::vector<const char*> extensions = {
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
	};
	std::size_t additionalExtensionCount = extensions.size();
	extensions.resize(additionalExtensionCount + count);

	if(!SDL_Vulkan_GetInstanceExtensions(window_.get(), &count, extensions.data() + additionalExtensionCount))
		SPDLOG_CRITICAL("Failed to get the additional extensions");

	VkInstanceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.pApplicationInfo = &app_,
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
	};

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);

	if(!SDL_Vulkan_CreateSurface(window_.get(), instance_, &surface_))
		SPDLOG_CRITICAL("Failed to create surface");
}

void eos::GraphicsVulkan::swap() {

}

void eos::GraphicsVulkan::vsync(bool value) {

}

void eos::GraphicsVulkan::resize(int width, int height) {

}

glm::ivec2 eos::GraphicsVulkan::size() const {
	glm::ivec2 size;
	SDL_Vulkan_GetDrawableSize(window_.get(), &size[0], &size[1]);
	return size;
}
