#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <vector>

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "main/game_logic.h"
#include "graphics/backend/vulkan/global_state.h"
#include "graphics/backend/vulkan/render_state.h"
#include "graphics/frontend/instance.h"
#include "graphics/frontend/pipeline.h"
#include "graphics/frontend/render_stage.h"

Pipeline::Pipeline() = default;

Pipeline::~Pipeline() = default;

void record_command_buffer(const VkCommandBuffer buffer, uint32_t image_index)
{
	//TODO(ches) create a pipeline struct
	//TODO(ches) re-enable this when we have render pass, frame buffers, etc.
#if 0
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(buffer, &begin_info) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to begin recording a command buffer");
	}

	const VkExtent2D extent = g_vk_state.swap_chain.extent;

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = render_pass;
	renderPassInfo.framebuffer = frame_buffers[image_index];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(buffer, &renderPassInfo,
		VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		graphics_pipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(buffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(buffer, 0, 1, &scissor);

	vkCmdDraw(buffer, 3, 1, 0, 0);

	//TODO(ches) draw UI here

	vkCmdEndRenderPass(g_vk_state.render.current_command_buffer());

	if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
	{
		LOG_FATAL("Failed recording command buffer");
	}
#endif
}

void Pipeline::render(Scene& scene, ShaderMap& shaders)
{
	RenderState* render_state = &g_vk_state.render;

	if (!render_state->rendering_active)
	{
		return;
	}

	const VkDevice device = g_vk_state.device.logical;
	const VkSwapchainKHR swap_chain = g_vk_state.swap_chain.current_swap_chain;
	const uint32_t current_frame = render_state->current_frame;

	vkWaitForFences(device, 1,
		&render_state->frame_in_flight_fences[current_frame],
		VK_TRUE, UINT64_MAX);

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(device, swap_chain, UINT64_MAX,
		render_state->image_available_semaphores[current_frame],
		VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR
		|| result == VK_SUBOPTIMAL_KHR
		|| g_vk_state.window.resized)
	{
		g_vk_state.window.resized = false;
		g_game_logic->render_instance->recreate_swap_chain();
		return;
	}
	else if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to acquire swap chain image");
	}

	vkResetFences(device, 1,
		&render_state->frame_in_flight_fences[current_frame]);

	const VkCommandBuffer buffer =
		g_vk_state.render.current_command_buffer();
	vkResetCommandBuffer(buffer, 0);

	record_command_buffer(buffer, image_index);

	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = {
		render_state->image_available_semaphores[current_frame]
	};
	VkPipelineStageFlags wait_stages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &buffer;

	VkSemaphore signal_semaphores[] = {
		render_state->render_finished_semaphores[current_frame]
	};
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;
	if (vkQueueSubmit(g_vk_state.device.graphics_queue, 1,
		&submit_info, render_state->frame_in_flight_fences[current_frame])
		!= VK_SUCCESS)
	{
		LOG_FATAL("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapChains[] = {
		g_vk_state.swap_chain.current_swap_chain
	};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapChains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	vkQueuePresentKHR(g_vk_state.device.graphics_queue, &present_info);

	render_state->current_frame =
		(current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

#endif