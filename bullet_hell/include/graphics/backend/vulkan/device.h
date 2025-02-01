#pragma once

#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    [[nodiscard]]
    bool has_all_values() const noexcept
    {
        return graphics_family.has_value()
            && present_family.has_value();
    }
};

/// <summary>
/// Information about swap chain support.
/// </summary>
struct SwapChainSupport
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats{};
    std::vector<VkPresentModeKHR> present_modes{};
};

struct Device
{
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device = VK_NULL_HANDLE;
    VkQueue graphics_queue = nullptr;
    VkQueue present_queue = nullptr;
    QueueFamilyIndices indices;
    VkDescriptorPool descriptor_pool = nullptr;
    SwapChainSupport swap_chain_support;
};

namespace DeviceUtil
{
    /// <summary>
    /// Check what kind of swap chain support the device has.
    /// </summary>
    /// <param name="device">The device we are checking.</param>
    /// <param name="surface">The surface we are trying to support.</param>
    /// <returns></returns>
    [[nodiscard]]
    SwapChainSupport check_swap_chain_support(const VkPhysicalDevice device,
        const VkSurfaceKHR surface);

    /// <summary>
    /// Find all the queue family indices we care about for a device.
    /// </summary>
    /// <param name="device">The device we are checking.</param>
    /// <param name="surface">The surface we are trying to support.</param>
    /// <returns>What queue family indices we could find.</returns>
    [[nodiscard]]
    QueueFamilyIndices find_queue_families(const VkPhysicalDevice device,
        const VkSurfaceKHR surface);

    /// <summary>
    /// Calculate a score to represent how much desireable a device is. Will
    /// be zero if it's not usable for us.
    /// </summary>
    /// <param name="device">The device we are rating.</param>
    /// <param name="surface">The surface we are trying to support.</param>
    /// <returns>A score for the device.</returns>
    [[nodiscard]]
    int rate_device(const VkPhysicalDevice device,
        const VkSurfaceKHR surface);

    void set_up_device();

    /// <summary>
    /// Checks if a device supports all the extensions we need.
    /// </summary>
    /// <param name="device">The device to check.</param>
    /// <returns>If it supports the required extensions.</returns>
    [[nodiscard]]
    bool supports_required_extensions(const VkPhysicalDevice device);

}

#endif