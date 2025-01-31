#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <format>

#include "revision.h"
#include "debugging/logger.h"
#include "graphics/backend/vulkan/global_state.h"
#include "graphics/frontend/buffer.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/instance.h"
#include "graphics/frontend/texture.h"
#include "graphics/gui/ui.h"
#include "main/game_logic.h"
#include "memory/memory_util.h"

#include <vulkan/vulkan.h>

#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"

Configuration Instance::configuration;

#pragma region Constants
/// <summary>
/// The validation layers that we want to enable.
/// </summary>
const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

#if _DEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

#pragma endregion

#pragma region Callbacks
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data)
{
    std::string tag;
    switch (message_type)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        // Not relate dto specification or performance
        tag = "[General]";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        // Violates specification or a possible mistake
        tag = "[Validation]";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        // Potential non-optimal use of Vulkan
        tag = "[Performance]";
        break;
    default:
        tag = "[Other]";
        break;
    }
    
    const std::string message = std::format("{} Validation layer message: {}", 
        tag, callback_data->pMessage);

    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        // Diagnostic message
        LOG_TAGGED("Debug", message);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    default:
        // Basic info like creation of a resource
        LOG_INFO(message);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        // Very likely a bug
        LOG_WARNING(message);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        // Invalid behavior that may cause crashes
        LOG_ERROR(message);
        break;
    }

    return VK_FALSE;
}
#pragma endregion

#pragma region Utility functions
/// <summary>
/// Attemt to load and call an extension function.
/// </summary>
/// <typeparam name="...ParamTypes">The types of the parameters.</typeparam>
/// <typeparam name="ResultType">The return type.</typeparam>
/// <param name="instance">The Vulkan instance to use.</param>
/// <param name="function_name">The name of the function to call.</param>
/// <param name="result">An out paramter where the result will be stored.
/// If there is no result, this should be nullptr.</param>
/// <param name="...params">Parameters to the extension function.</param>
template <typename ResultType, typename... ParamTypes>
void call_extension_function(VkInstance instance, 
    const char* function_name, ResultType&& result,
    ParamTypes... params)
{
    PFN_vkVoidFunction func = vkGetInstanceProcAddr(instance, function_name);

    if (func == nullptr)
    {
        LOG_ERROR("Invalid extension function called: "
            + std::string(function_name));
        return;
    }

    using ActualReturn = std::conditional<
        std::is_same_v<ResultType, std::nullptr_t>, void, ResultType>::type;

    typedef ActualReturn(VKAPI_PTR* ActualSignature)(ParamTypes...);

    ActualSignature actual_func = (ActualSignature)func;

    if constexpr (std::is_same_v<ResultType, std::nullptr_t>)
    {
        actual_func(std::forward<ParamTypes>(params)...);
    }
    else
    {
        result = (ResultType)
            actual_func(std::forward<ParamTypes>(params)...);
    }
}

/// <summary>
/// Check that we have all the validation layers that we expect to have.
/// </summary>
/// <returns>Whether we have all the validation layers we want.</returns>
[[nodiscard]] bool check_validation_layer_support()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const auto& layer_name : VALIDATION_LAYERS)
    {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers)
        {
            if (strcmp(layer_name, layer_properties.layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }
        if (!layer_found)
        {
            LOG_INFO("We are missing (at least) the validation layer "
                + std::string(layer_name));
            return false;
        }
    }
    return true;
}

[[nodiscard]]
std::vector<const char*> get_required_extensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions,
        glfw_extensions + glfw_extension_count);
    if (ENABLE_VALIDATION_LAYERS)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

/// <summary>
/// Fetch a list of available extensions.
/// </summary>
/// <returns>All the available Vulkan extensions.</returns>
[[nodiscard]]
std::vector<VkExtensionProperties> get_available_extensions();

/// <summary>
/// Initialize and fill out the debug create info structure.
/// </summary>
/// <param name="create_info">The structure to fill out.</param>
void popualate_debug_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
    create_info.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = nullptr;
}

[[nodiscard]]
VkExtent2D select_extent(const VkSurfaceCapabilitiesKHR& capabilities,
    int window_width, int window_height)
{
    if (capabilities.currentExtent.width
        != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(window_width),
            static_cast<uint32_t>(window_height)
        };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

#pragma endregion

#pragma region Instance
Instance::Instance()
	: deletion_queue{}
	, shader_map{}
	, pipeline_manager{ nullptr }
    , pipeline{ nullptr }
{
    //NOTE(ches) for the Vulkan diagnostic messages in our debug callback
    Logger::set_display_flags("Debug", FLAG_WRITE_TO_DEBUGGER);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    if (!glfwVulkanSupported())
    {
        LOG_FATAL("Vulkan is not supported on this system!");
    }

    if (ENABLE_VALIDATION_LAYERS && !check_validation_layer_support())
    {
        LOG_FATAL("We expect validation layers but don't have them");
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "Bullet Hell";
    app_info.applicationVersion =
        VK_MAKE_API_VERSION(1, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    app_info.pEngineName = "Ikala";
    app_info.engineVersion =
        VK_MAKE_API_VERSION(1, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        create_info.enabledLayerCount =
            static_cast<uint32_t>(VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

        popualate_debug_info(debug_create_info);
        create_info.pNext = &debug_create_info;
    }
    else
    {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    std::vector<const char*> extensions = get_required_extensions();
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&create_info, nullptr, &g_vk_state.instance)
        != VK_SUCCESS)
    {
        LOG_FATAL("Failed to create Vulkan instance");
    }

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    popualate_debug_info(debug_create_info);

    VkResult debug_creation_result = VK_SUCCESS;

    VkAllocationCallbacks* allocator = nullptr;

    call_extension_function(g_vk_state.instance, "vkCreateDebugUtilsMessengerEXT",
        &debug_creation_result,
        g_vk_state.instance, &debug_create_info, allocator,
        &g_vk_state.debug_messenger);

    if (debug_creation_result != VK_SUCCESS)
    {
        LOG_ERROR("Failed attaching a debug callback for validation logs");
    }

}

Instance::~Instance()
{
	safe_delete(pipeline);

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void delete_resource(DeletionQueue::Entry entry)
{
	
}

void Instance::initialize()
{
    //TODO(ches) create device

   /* g_vk_state.swap_chain_support = check_swap_chain_support(
        g_vk_state.device.physical_device,
        g_vk_state.window_state.surface);*/
}

void Instance::create_swap_chain()
{
    //TODO(ches) create surface first

    const Device& device = g_vk_state.device;
    const SwapChainSupport& support = g_vk_state.swap_chain_support;

    VkExtent2D extent = select_extent(support.capabilities,
        g_vk_state.window_state.width, g_vk_state.window_state.height);
    uint32_t image_count = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0
        && image_count > support.capabilities.maxImageCount)
    {
        image_count = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = g_vk_state.window_state.surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = g_vk_state.window_state.surface_format->format;
    create_info.imageColorSpace = g_vk_state.window_state.surface_format->colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.preTransform = support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = g_vk_state.window_state.present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = g_vk_state.swap_chain.last_swap_chain;

    QueueFamilyIndices indices = find_queue_families(device.physical_device,
        g_vk_state.window_state.surface);

    uint32_t queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    if (indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    if (vkCreateSwapchainKHR(device.logical_device, &create_info, nullptr,
        &g_vk_state.swap_chain.vulkan_swap_chain) != VK_SUCCESS)
    {
        LOG_FATAL("Failed to create swap chain");
    }

    vkGetSwapchainImagesKHR(device.logical_device,
        g_vk_state.swap_chain.vulkan_swap_chain, &image_count, nullptr);
    g_vk_state.swap_chain.images.resize(image_count);
    vkGetSwapchainImagesKHR(device.logical_device,
        g_vk_state.swap_chain.vulkan_swap_chain, &image_count,
        g_vk_state.swap_chain.images.data());

    g_vk_state.swap_chain.image_format = g_vk_state.window_state.surface_format->format;
    g_vk_state.swap_chain.extent = extent;

    g_vk_state.swap_chain.image_views.resize(g_vk_state.swap_chain.images.size());

    for (size_t i = 0; i < g_vk_state.swap_chain.images.size(); i++)
    {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = g_vk_state.swap_chain.images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = g_vk_state.swap_chain.image_format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.logical_device, &create_info, nullptr,
            &g_vk_state.swap_chain.image_views[i]) != VK_SUCCESS)
        {
            LOG_FATAL("Failed to create image views for the swap chain");
        }
    }
}

void Instance::initialize_pipeline_manager()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForVulkan(g_game_logic->window->handle, true);

    pipeline_manager = ALLOC PipelineManager(*g_game_logic->window,
        &deletion_queue, shader_map);
    pipeline = pipeline_manager->get_pipeline(RenderConfigPrefab::JUST_GUI);
}

void Instance::process_resources()
{
	
}

void Instance::recreate_swap_chain()
{
    g_vk_state.swap_chain.last_swap_chain = g_vk_state.swap_chain.vulkan_swap_chain;

    const VkDevice& device = g_vk_state.device.logical_device;

    vkDeviceWaitIdle(device);
    pipeline_manager->destroy_frame_buffers();

    for (auto view : g_vk_state.swap_chain.image_views)
    {
        vkDestroyImageView(device, view, nullptr);
    }
    g_vk_state.swap_chain.image_views.clear();

    vkDestroySwapchainKHR(device, g_vk_state.swap_chain.vulkan_swap_chain, nullptr);

    //TODO(ches) render_state->recreate_synchronization_objects();

    create_swap_chain();
    pipeline_manager->create_frame_buffers();
}

void Instance::render(Scene& scene)
{
	pipeline->render(scene, shader_map);
}

void Instance::resize(int width, int height)
{
	pipeline_manager->resize(width, height);
	ImVec2& display_size = ImGui::GetMainViewport()->Size;
	display_size.x = width;
	display_size.y = height;
}

void Instance::setup_data(Scene& scene)
{
	pipeline_manager->setup_data(scene);
}

void Instance::swap_pipeline(RenderConfig config)
{
	pipeline = pipeline_manager->get_pipeline(config);
}

void Instance::set_filter(const std::string_view shader_path)
{
	std::vector<Shader::Module> module_data;

	module_data.emplace_back(std::format("{}.frag", shader_path),
		Shader::Type::FRAGMENT);
	module_data.emplace_back(std::format("{}.vert", shader_path),
		Shader::Type::VERTEX);

	Shader* shader = ALLOC Shader(module_data);
	shader->uniforms.create_uniform("screen_texture");

	shader_map.add_shader(RenderStage::Type::FILTER, shader);
}

#pragma endregion

#endif