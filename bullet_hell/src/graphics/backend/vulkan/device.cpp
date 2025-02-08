#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <map>
#include <set>
#include <string>
#include <vector>

#include "debugging/logger.h"
#include "graphics/backend/vulkan/device.h"
#include "graphics/backend/vulkan/global_state.h"

/// <summary>
/// The extensions that we require on a device.
/// </summary>
const std::vector<const char*> REQUIRED_EXTENSIONS = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

namespace DeviceUtil
{
	/// <summary>
	/// Set the window surface up with present modes and surface format based
	/// on the selected device.
	/// </summary>
	void configure_surface()
	{
		SwapChainSupport& swap_chain_support = g_vk_state.device.swap_chain_support;

		swap_chain_support =
			check_swap_chain_support(g_vk_state.device.physical,
				g_vk_state.window.surface);

		if (swap_chain_support.present_modes.empty())
		{
			LOG_WARNING("We don't have any available present modes");
		}

		for (const auto& availablePresentMode : swap_chain_support.present_modes)
		{
			//NOTE(ches) reduces latency, but maybe at the expense of some power
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				// NOTE(ches) VK_PRESENT_MODE_FIFO_KHR is guaranteed, and our default
				g_vk_state.window.present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}
		
		LOG_ASSERT(swap_chain_support.formats.size() > 0
			&& "We require available surface formats");

		VkSurfaceFormatKHR& format = g_vk_state.window.surface_format;

		bool found_ideal_format = false;
		for (const auto& choice : swap_chain_support.formats)
		{
			if (choice.format == VK_FORMAT_B8G8R8A8_UNORM
				&& choice.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				format.colorSpace = choice.colorSpace;
				format.format = choice.format;

				found_ideal_format = true;
				break;
			}
		}

		if (!found_ideal_format)
		{
			format.colorSpace = swap_chain_support.formats[0].colorSpace;
			format.format = swap_chain_support.formats[0].format;
		}

	}

	/// <summary>
	/// Set up the device queues.
	/// </summary>
	void create_queues()
	{
		const uint32_t queue_index = 0;
		vkGetDeviceQueue(g_vk_state.device.logical,
			g_vk_state.device.indices.present_family.value(),
			queue_index, &g_vk_state.device.present_queue);
		vkGetDeviceQueue(g_vk_state.device.logical,
			g_vk_state.device.indices.graphics_family.value(),
			queue_index, &g_vk_state.device.graphics_queue);
	}

	/// <summary>
	/// Select a logical device to use among the system devices, and store it
	/// in the game state. Might have a fatal problem if we can't pick a
	/// device.
	/// </summary>
	void select_logical_device()
	{
		g_vk_state.device.indices = find_queue_families(
			g_vk_state.device.physical, g_vk_state.window.surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			g_vk_state.device.indices.graphics_family.value(),
			g_vk_state.device.indices.present_family.value()
		};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures device_features{};
		//TODO(ches) Select device features

		VkDeviceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = queueCreateInfos.data();
		create_info.queueCreateInfoCount =
			static_cast<uint32_t>(queueCreateInfos.size());;
		create_info.pEnabledFeatures = &device_features;
		create_info.enabledExtensionCount =
			static_cast<uint32_t>(REQUIRED_EXTENSIONS.size());
		create_info.ppEnabledExtensionNames = REQUIRED_EXTENSIONS.data();

		if (ENABLE_VALIDATION_LAYERS)
		{
			create_info.enabledLayerCount =
				static_cast<uint32_t>(VALIDATION_LAYERS.size());
			create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		}
		else
		{
			create_info.enabledLayerCount = 0;
		}

		if (vkCreateDevice(g_vk_state.device.physical, &create_info,
			nullptr, &g_vk_state.device.logical) != VK_SUCCESS)
		{
			LOG_FATAL("Could not create a logical device");
		}

	}

	/// <summary>
	/// Select a physical device to use among the system devices, and store it
	/// in the game state. Might have a fatal problem if we can't find a GPU.
	/// </summary>
	void select_physical_device()
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(g_vk_state.instance, &device_count,
			nullptr);

		if (device_count == 0)
		{
			LOG_FATAL("No GPUs support Vulkan");
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(g_vk_state.instance, &device_count,
			devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices)
		{
			int score = rate_device(device, g_vk_state.window.surface);
			if (score > 0)
			{
				candidates.insert(std::make_pair(score, device));
			}
		}

		if (candidates.empty()) {
			LOG_FATAL("No GPUs are suitable for this program");
		}

		g_vk_state.device.physical = candidates.rbegin()->second;
	}


	[[nodiscard]]
	SwapChainSupport check_swap_chain_support(const VkPhysicalDevice device,
		const VkSurfaceKHR surface)
	{
		SwapChainSupport details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
			&details.capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
			nullptr);

		if (format_count != 0)
		{
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
				&format_count, details.formats.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
			&present_mode_count, nullptr);

		if (present_mode_count != 0)
		{
			details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
				&present_mode_count, details.present_modes.data());
		}

		return details;
	}

	[[nodiscard]]
	QueueFamilyIndices find_queue_families(const VkPhysicalDevice device,
		const VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices{};

		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
			nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
			queue_families.data());

		int i = 0;
		for (const auto& queue_family : queue_families)
		{
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
			}
			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
				&present_support);

			if (present_support)
			{
				indices.present_family = i;
			}

			++i;
			if (indices.has_all_values())
			{
				break;
			}
		}

		return indices;
	}

	int rate_device(const VkPhysicalDevice device, const VkSurfaceKHR surface)
	{
		int score = 0;

		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(device, &device_properties);
		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceFeatures(device, &device_features);

		if (!device_features.geometryShader)
		{
			return 0;
		}

		QueueFamilyIndices queue_families = find_queue_families(device, surface);

		if (!queue_families.has_all_values())
		{
			return 0;
		}

		if (!supports_required_extensions(device))
		{
			return 0;
		}

		SwapChainSupport swap_chain_support = check_swap_chain_support(device, surface);
		if (swap_chain_support.formats.empty()
			|| swap_chain_support.present_modes.empty())
		{
			return 0;
		}

		//NOTE(ches) Discrete GPU is much better than on-chip
		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		//NOTE(ches) Largest possible size of a texture
		score += device_properties.limits.maxImageDimension2D;

		return score;
	}

	void set_up_device()
	{
		select_physical_device();
		select_logical_device();
		create_queues();
		configure_surface();
	}

	[[nodiscard]]
	bool supports_required_extensions(const VkPhysicalDevice device)
	{
		uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr,
			&extension_count, nullptr);

		std::vector<VkExtensionProperties> available_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
			available_extensions.data());

		std::set<std::string> required_extensions(REQUIRED_EXTENSIONS.begin(),
			REQUIRED_EXTENSIONS.end());

		for (const auto& extension : available_extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}
}

#endif