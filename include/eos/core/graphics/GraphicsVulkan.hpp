//
// Created by jakob on 24.09.21.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>
#include "eos/core/graphics/Graphics.hpp"

namespace eos {

	class GraphicsVulkan : public Graphics {
	public:
		~GraphicsVulkan() override;

		void setup(std::string_view title) override;

		void instantiate(std::experimental::observer_ptr<SDL_Window> window) override;

		void draw_frame() override;

		void vsync(bool value) override;

		void resize(int width, int height) override;

		[[nodiscard]] glm::ivec2 size() const override;

	private:
		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			[[nodiscard]] bool isComplete() const {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		void create_instance();
		void setup_debug_messenger();
		void create_surface();
		void pick_physical_device();
		void create_logical_device();
		void create_swapchain();
		void create_image_views();

		bool is_device_suitable(VkPhysicalDevice physicalDevice);
		bool check_device_extensions_support(VkPhysicalDevice physicalDevice);
		QueueFamilyIndices find_queue_families(VkPhysicalDevice physicalDevice);
		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);
		std::vector<const char*> get_required_extensions();
		bool check_validation_layer_support();

		static VkDebugUtilsMessengerCreateInfoEXT get_debug_utils_messenger_create_info();
		// helper function for creating debug utils messenger
		static VkResult
		CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		                             const VkAllocationCallbacks* pAllocator,
		                             VkDebugUtilsMessengerEXT* pDebugMessenger);
		// helper function for destroying debug utils messenger
		static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
		                                          const VkAllocationCallbacks* pAllocator);

		const std::vector<const char*> requiredValidationLayers_{
				"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> requiredDeviceExtensions_{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		VkApplicationInfo applicationInfo_;
		VkInstanceCreateInfo instanceCreateInfo_;
		VkInstance instance_;
		VkDebugUtilsMessengerEXT debugMessenger_;
		VkSurfaceKHR surface_;
		VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
		VkDevice device_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;
		VkSwapchainKHR swapchain_;
		std::vector<VkImage> swapchainImages_;
		std::vector<VkImageView> swapchainImageViews_;
		VkFormat swapchainImageFormat_;
		VkExtent2D swapchainExtent_;
	};

}