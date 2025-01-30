#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <set>
#include <string>

#include "graphics/backend/vulkan/device.h"

/// <summary>
/// The extensions that we require on a device.
/// </summary>
const std::vector<const char*> REQUIRED_EXTENSIONS = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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

#endif