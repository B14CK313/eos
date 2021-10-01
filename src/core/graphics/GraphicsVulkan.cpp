//
// Created by jakob on 20.09.21.
//

#include <set>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <SDL_vulkan.h>
#include "eos/utils.hpp"
#include "eos/core/graphics/GraphicsVulkan.hpp"

eos::GraphicsVulkan::~GraphicsVulkan() {
	vkDeviceWaitIdle(device_);
	for (std::size_t i{0}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroySemaphore(device_, renderFinishedSemaphores_[i], nullptr);
		vkDestroySemaphore(device_, imageAvailableSemaphores_[i], nullptr);
		vkDestroyFence(device_, inFlightFences_[i], nullptr);
	}
	vkDestroyCommandPool(device_, commandPool_, nullptr);
	for (auto framebuffer: swapchainFramebuffers_) {
		vkDestroyFramebuffer(device_, framebuffer, nullptr);
	}
	vkDestroyPipeline(device_, graphicsPipeline_, nullptr);
	vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
	vkDestroyRenderPass(device_, renderPass_, nullptr);
	for (auto imageView: swapchainImageViews_) {
		vkDestroyImageView(device_, imageView, nullptr);
	}
	vkDestroySwapchainKHR(device_, swapchain_, nullptr);
	vkDestroyDevice(device_, nullptr);
	vkDestroySurfaceKHR(instance_, surface_, nullptr);
#ifndef NDEBUG
	DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
#endif //NDEBUG
	vkDestroyInstance(instance_, nullptr);
}

void eos::GraphicsVulkan::setup(std::string_view title) {
	applicationInfo_ = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = title.data(),
			.applicationVersion = 0, // FIXME: use actual version here
			.pEngineName = "eos",
			.engineVersion = 0, // FIXME: use actual version here
			.apiVersion = VK_API_VERSION_1_0,
	};

	instanceCreateInfo_ = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &applicationInfo_,
	};
}

void eos::GraphicsVulkan::instantiate(std::experimental::observer_ptr<SDL_Window> window) {
	window_ = window;

	create_instance();

#ifndef NDEBUG
	setup_debug_messenger();
#endif //NDEBUG

	create_surface();
	pick_physical_device();
	create_logical_device();
	create_swapchain();
	create_image_views();
	create_render_pass();
	create_graphics_pipeline();
	create_framebuffers();
	create_command_pool();
	create_command_buffers();
	create_semaphores_and_fences();
}

void eos::GraphicsVulkan::draw_frame() {
	vkWaitForFences(device_, 1, &inFlightFences_[currentFrame_], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t imageIndex;
	if (vkAcquireNextImageKHR(device_, swapchain_, std::numeric_limits<uint64_t>::max(),
	                          imageAvailableSemaphores_[currentFrame_],
	                          VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS) {
		SPDLOG_ERROR("Error while acquiring next image");
	}

	std::array<VkSemaphore, 1> waitSemaphores{imageAvailableSemaphores_[currentFrame_]};
	std::array<VkPipelineStageFlags, 1> waitStages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	std::array<VkSemaphore, 1> signalSemaphores{renderFinishedSemaphores_[currentFrame_]};
	VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = waitSemaphores.data(),
			.pWaitDstStageMask = waitStages.data(),
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffers_[imageIndex],
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = signalSemaphores.data(),
	};

	if (inFlightImages_[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device_, 1, &inFlightImages_[imageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
	}

	vkResetFences(device_, 1, &inFlightFences_[currentFrame_]);

	if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFences_[currentFrame_]) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to submit draw command buffer");
		throw;
	}

	std::array<VkSwapchainKHR, 1> swapchains{swapchain_};
	VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = signalSemaphores.size(),
			.pWaitSemaphores = signalSemaphores.data(),
			.swapchainCount = swapchains.size(),
			.pSwapchains = swapchains.data(),
			.pImageIndices = &imageIndex,
			.pResults = nullptr,
	};

	if (vkQueuePresentKHR(presentQueue_, &presentInfo) != VK_SUCCESS) {
		SPDLOG_ERROR("Error while presenting queue");
	}

	currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
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


void eos::GraphicsVulkan::create_instance() {
	auto requiredExtensions = get_required_extensions();

	instanceCreateInfo_.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	instanceCreateInfo_.ppEnabledExtensionNames = requiredExtensions.data();

#ifndef NDEBUG
	if (!check_validation_layer_support()) {
		SPDLOG_ERROR("Validation layers requested, but not available");
		throw;
	}

	auto creationDebugUtilsMessengerCreateInfo = get_debug_utils_messenger_create_info();

	instanceCreateInfo_.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers_.size());
	instanceCreateInfo_.ppEnabledLayerNames = requiredValidationLayers_.data();
	instanceCreateInfo_.pNext = &creationDebugUtilsMessengerCreateInfo;
#else //NDEBUG
	instanceCreateInfo_.enabledLayerCount = 0;
	instanceCreateInfo_.ppEnabledLayerNames = nullptr;
	instanceCreateInfo_.pNext = nullptr;
#endif //NDEBUG

	if (vkCreateInstance(&instanceCreateInfo_, nullptr, &instance_) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create instance");
		throw;
	}
}

void eos::GraphicsVulkan::setup_debug_messenger() {
	auto debugUtilsMessengerCreateInfo = get_debug_utils_messenger_create_info();

	if (CreateDebugUtilsMessengerEXT(instance_, &debugUtilsMessengerCreateInfo, nullptr, &debugMessenger_) !=
	    VK_SUCCESS) {
		SPDLOG_ERROR("Failed to setup Vulkan debug messenger");
		throw;
	}
}

void eos::GraphicsVulkan::create_surface() {
	if (!SDL_Vulkan_CreateSurface(window_.get(), instance_, &surface_)) {
		SPDLOG_CRITICAL("Failed to create surface");
		throw;
	}
}

void eos::GraphicsVulkan::pick_physical_device() {
	uint32_t deviceCount;
	if (vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr) != VK_SUCCESS || deviceCount == 0) {
		SPDLOG_CRITICAL("Failed to get the number of devices with Vulkan support");
		throw;
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	if (vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data()) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get the devices with Vulkan support");
		throw;
	}

	for (const auto& device: devices) {
		if (is_device_suitable(device)) {
			physicalDevice_ = device;
			break;
		}
	}

	if (physicalDevice_ == VK_NULL_HANDLE) {
		SPDLOG_CRITICAL("Failed to find a suitable device");
		throw;
	}
}

void eos::GraphicsVulkan::create_logical_device() {
	QueueFamilyIndices indices = find_queue_families(physicalDevice_);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies{indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority{1.0f};
	for (auto queueFamily: uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority,
		};
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo deviceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions_.size()),
			.ppEnabledExtensionNames = requiredDeviceExtensions_.data(),
			.pEnabledFeatures = &deviceFeatures,
	};

#ifndef NDEBUG
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers_.size());
	deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers_.data();
#else //NDEBUG
	deviceCreateInfo.enabledLayerCount = 0;
#endif //NDEBUG

	if (vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create logical device");
		throw;
	}

	vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
	vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);
}

void eos::GraphicsVulkan::create_swapchain() {
	SwapChainSupportDetails swapchainSupport = query_swapchain_support(physicalDevice_);

	VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapchainSupport.formats);
	VkPresentModeKHR presentMode = choose_swap_present_mode(swapchainSupport.presentModes);
	VkExtent2D extent = choose_swap_extent(swapchainSupport.capabilities);

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
	if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface_,
			.minImageCount = imageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	};

	QueueFamilyIndices indices = find_queue_families(physicalDevice_);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily) {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	swapchainCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device_, &swapchainCreateInfo, nullptr, &swapchain_) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create swapchain");
		throw;
	}

	if (vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, nullptr) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get swapchain images count");
		throw;
	}
	swapchainImages_.resize(imageCount);
	vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, swapchainImages_.data());

	swapchainImageFormat_ = surfaceFormat.format;
	swapchainExtent_ = extent;
}

void eos::GraphicsVulkan::create_image_views() {
	swapchainImageViews_.resize(swapchainImages_.size());
	for (std::size_t i{0}; i < swapchainImages_.size(); ++i) {
		VkImageViewCreateInfo imageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = swapchainImages_[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = swapchainImageFormat_,
				.components = {
						.r = VK_COMPONENT_SWIZZLE_IDENTITY,
						.g = VK_COMPONENT_SWIZZLE_IDENTITY,
						.b = VK_COMPONENT_SWIZZLE_IDENTITY,
						.a = VK_COMPONENT_SWIZZLE_IDENTITY,
				},
				.subresourceRange = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel = 0,
						.levelCount = 1,
						.baseArrayLayer = 0,
						.layerCount = 1,
				}
		};

		if (vkCreateImageView(device_, &imageViewCreateInfo, nullptr, &swapchainImageViews_[i]) != VK_SUCCESS) {
			SPDLOG_CRITICAL("Failed to create image view");
			throw;
		}
	}
}


void eos::GraphicsVulkan::create_render_pass() {
	VkAttachmentDescription colorAttachmentDescription{
			.format = swapchainImageFormat_,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};
	VkAttachmentReference colorAttachmentReference{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkSubpassDescription subpassDescription{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentReference,
	};
	VkSubpassDependency subpassDependency{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};
	VkRenderPassCreateInfo renderPassCreateInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &colorAttachmentDescription,
			.subpassCount = 1,
			.pSubpasses = &subpassDescription,
			.dependencyCount = 1,
			.pDependencies = &subpassDependency,
	};

	if (vkCreateRenderPass(device_, &renderPassCreateInfo, nullptr, &renderPass_) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create render pass");
		throw;
	}
}

void eos::GraphicsVulkan::create_graphics_pipeline() {
	std::vector<char> vertShaderCode{};
	std::vector<char> fragShaderCode{};

	eos::utils::load_file("./shaders/vert.spv", vertShaderCode);
	eos::utils::load_file("./shaders/frag.spv", fragShaderCode);

	VkShaderModule vertShaderModule = create_shader_module(vertShaderCode);
	VkShaderModule fragShaderModule = create_shader_module(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertPipelineShaderStageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertShaderModule,
			.pName = "main",
			.pSpecializationInfo = nullptr // optional constant options
	};
	VkPipelineShaderStageCreateInfo fragPipelineShaderStageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragShaderModule,
			.pName = "main",
			.pSpecializationInfo = nullptr
	};

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{vertPipelineShaderStageCreateInfo,
	                                                            fragPipelineShaderStageCreateInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr,
	};
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
	};
	VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(swapchainExtent_.width),
			.height = static_cast<float>(swapchainExtent_.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
	};
	VkRect2D scissor{
			.offset = {0, 0},
			.extent = swapchainExtent_,
	};
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor,
	};
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f,
	};
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE,
	};
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			                  VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &pipelineColorBlendAttachmentState,
			.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
	};
	std::array<VkDynamicState, 2> dynamicStates{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH,
	};
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 2,
			.pDynamicStates = dynamicStates.data(),
	};
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr,
	};
	if (vkCreatePipelineLayout(device_, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create pipeline layout");
		throw;
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shaderStages.data(),
			.pVertexInputState = &vertexInputStateCreateInfo,
			.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
			.pViewportState = &pipelineViewportStateCreateInfo,
			.pRasterizationState = &pipelineRasterizationStateCreateInfo,
			.pMultisampleState = &pipelineMultisampleStateCreateInfo,
			.pDepthStencilState = nullptr,
			.pColorBlendState = &pipelineColorBlendStateCreateInfo,
			.pDynamicState = nullptr,
			.layout = pipelineLayout_,
			.renderPass = renderPass_,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
	};

	if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline_) !=
	    VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create graphics pipeline");
		throw;
	}

	vkDestroyShaderModule(device_, fragShaderModule, nullptr);
	vkDestroyShaderModule(device_, vertShaderModule, nullptr);
}

void eos::GraphicsVulkan::create_framebuffers() {
	swapchainFramebuffers_.resize(swapchainImageViews_.size());
	for (std::size_t i{0}; i < swapchainImageViews_.size(); ++i) {
		std::array<VkImageView, 1> attachments{
				swapchainImageViews_[i]
		};

		VkFramebufferCreateInfo framebufferCreateInfo{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = renderPass_,
				.attachmentCount = 1,
				.pAttachments = attachments.data(),
				.width = swapchainExtent_.width,
				.height = swapchainExtent_.height,
				.layers = 1,
		};

		if (vkCreateFramebuffer(device_, &framebufferCreateInfo, nullptr, &swapchainFramebuffers_[i]) != VK_SUCCESS) {
			SPDLOG_CRITICAL("Failed to create framebuffer");
			throw;
		}
	}
}

void eos::GraphicsVulkan::create_command_pool() {
	QueueFamilyIndices queueFamilyIndices = find_queue_families(physicalDevice_);

	VkCommandPoolCreateInfo commandPoolCreateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = 0,
			.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
	};

	if (vkCreateCommandPool(device_, &commandPoolCreateInfo, nullptr, &commandPool_) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create command pool");
		throw;
	}
}

void eos::GraphicsVulkan::create_command_buffers() {
	commandBuffers_.resize(swapchainFramebuffers_.size());
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = commandPool_,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size()),
	};

	if (vkAllocateCommandBuffers(device_, &commandBufferAllocateInfo, commandBuffers_.data()) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to allocate command buffers");
		throw;
	}

	for (std::size_t i{0}; i < commandBuffers_.size(); ++i) {
		VkCommandBufferBeginInfo commandBufferBeginInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.flags = 0,
				.pInheritanceInfo = nullptr,
		};

		if (vkBeginCommandBuffer(commandBuffers_[i], &commandBufferBeginInfo) != VK_SUCCESS) {
			SPDLOG_CRITICAL("Failed to begin recording command buffer");
			throw;
		}

		VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		VkRenderPassBeginInfo renderPassBeginInfo{
				.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				.renderPass = renderPass_,
				.framebuffer = swapchainFramebuffers_[i],
				.renderArea{
						.offset = {0, 0},
						.extent = swapchainExtent_,
				},
				.clearValueCount = 1,
				.pClearValues = &clearColor,
		};

		vkCmdBeginRenderPass(commandBuffers_[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);
		vkCmdDraw(commandBuffers_[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers_[i]);
		if (vkEndCommandBuffer(commandBuffers_[i]) != VK_SUCCESS) {
			SPDLOG_CRITICAL("Failed to record command buffer");
			throw;
		}
	}
}

void eos::GraphicsVulkan::create_semaphores_and_fences() {
	inFlightImages_.resize(swapchainImages_.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};

	VkFenceCreateInfo fenceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};

	for (std::size_t i{0}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		if (!(vkCreateSemaphore(device_, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores_[i]) == VK_SUCCESS &&
		      vkCreateSemaphore(device_, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores_[i]) == VK_SUCCESS)) {
			SPDLOG_CRITICAL("Failed to create semaphore for a frame");
			throw;
		}
		if (vkCreateFence(device_, &fenceCreateInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
			SPDLOG_CRITICAL("Failed to create fence for a frame");
			throw;
		}
	}
}

std::vector<const char*> eos::GraphicsVulkan::get_required_extensions() {
	unsigned int requiredExtensionCount;
	if (!SDL_Vulkan_GetInstanceExtensions(window_.get(), &requiredExtensionCount, nullptr)) {
		SPDLOG_CRITICAL("Failed to get the number of required extensions");
		throw;
	}
	std::vector<const char*> requiredExtensions(requiredExtensionCount);
	if (!SDL_Vulkan_GetInstanceExtensions(window_.get(), &requiredExtensionCount, requiredExtensions.data())) {
		SPDLOG_CRITICAL("Failed to get the required extensions");
		throw;
	}

#ifndef NDEBUG
	requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif //NDEBUG

	return requiredExtensions;
}

bool eos::GraphicsVulkan::is_device_suitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = find_queue_families(device);

	bool extensionsSupported = check_device_extensions_support(device);
	bool swapchainAdequate{false};
	if (extensionsSupported) {
		SwapChainSupportDetails swapchainSupport = query_swapchain_support(device);
		swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapchainAdequate;
}

eos::GraphicsVulkan::QueueFamilyIndices eos::GraphicsVulkan::find_queue_families(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// TODO: https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Window_surface
	// Note that it's very likely that these end up being the same queue family after all, but throughout the program we
	// will treat them as if they were separate queues for a uniform approach. Nevertheless, you could add logic to
	// explicitly prefer a physical device that supports drawing and presentation in the same queue for improved
	// performance.
	for (uint32_t i{0}; i < queueFamilyCount; ++i) {
		const auto& queueFamily = queueFamilies[i];
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}
	}

	return indices;
}

VkSurfaceFormatKHR
eos::GraphicsVulkan::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat: availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
		    availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR
eos::GraphicsVulkan::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode: availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D eos::GraphicsVulkan::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		SDL_Vulkan_GetDrawableSize(window_.get(), &width, &height);

		VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
		                                capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
		                                 capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

eos::GraphicsVulkan::SwapChainSupportDetails eos::GraphicsVulkan::query_swapchain_support(VkPhysicalDevice device) {
	SwapChainSupportDetails details{};

	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get physical device surface capabilities");
		throw;
	}

	uint32_t formatCount;
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr) != VK_SUCCESS ||
	    formatCount == 0) {
		SPDLOG_CRITICAL("Failed to get physical device surface formats count");
		throw;
	}

	details.formats.resize(formatCount);
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data()) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get physical device surface formats");
		throw;
	}

	uint32_t presentModeCount;
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr) != VK_SUCCESS ||
	    presentModeCount == 0) {
		SPDLOG_CRITICAL("Failed to get physical device surface present modes count");
		throw;
	}

	details.presentModes.resize(presentModeCount);
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data()) !=
	    VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get physical device surface present modes");
		throw;
	}

	return details;
}

bool eos::GraphicsVulkan::check_device_extensions_support(VkPhysicalDevice physicalDevice) {
	uint32_t extensionCount;
	if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get device extension properties count");
		throw;
	}
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()) !=
	    VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to get device extension properties");
		throw;
	}

	std::set<std::string> requiredExtensions(requiredDeviceExtensions_.begin(), requiredDeviceExtensions_.end());
	for (const auto& extension: availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool eos::GraphicsVulkan::check_validation_layer_support() {
	uint32_t availableLayerCount;
	if (vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr) != VK_SUCCESS) {
		SPDLOG_ERROR("Failed to get the number of available layers");
	}
	std::vector<VkLayerProperties> availableLayers(availableLayerCount);
	if (vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()) != VK_SUCCESS) {
		SPDLOG_ERROR("Failed to get the available layers");
	}

	for (const auto& layerName: requiredValidationLayers_) {
		bool layerFound{false};

		for (const auto& layerProperties: availableLayers) {
			if (std::strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			SPDLOG_ERROR("Failed to get the required layers");
			return false;
		}
	}

	return true;
}

VkShaderModule eos::GraphicsVulkan::create_shader_module(const std::vector<char>& code) {
	VkShaderModuleCreateInfo shaderModuleCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = code.size(),
			.pCode = reinterpret_cast<const uint32_t*>(code.data()),
	};
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device_, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		SPDLOG_CRITICAL("Failed to create shader module");
		throw;
	}
	return shaderModule;
}

VkDebugUtilsMessengerCreateInfoEXT eos::GraphicsVulkan::get_debug_utils_messenger_create_info() {
	static auto debug_callback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	                                void* pUserData) -> VkBool32 {
		switch (messageSeverity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				SPDLOG_TRACE("Vulkan: {}", pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				SPDLOG_INFO("Vulkan: {}", pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				SPDLOG_WARN("Vulkan: {}", pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				SPDLOG_ERROR("Vulkan: {}", pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
				SPDLOG_CRITICAL("Vulkan: {}", pCallbackData->pMessage);
				break;
		}
		return VK_FALSE;
	};

	return {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debug_callback,
			.pUserData = nullptr, // Optional
	};
}

VkResult eos::GraphicsVulkan::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                           const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator,
                                                           VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void eos::GraphicsVulkan::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                        const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
	                                                                        "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
