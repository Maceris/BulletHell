#pragma once

#include "CascadeShadowSlice.h"

#include "gtc/matrix_transform.hpp"

CascadeShadowSlice::CascadeShadowSlice()
	: projection_view_matrix{ 1 }
	, split_distance{ 0 }
{}

constexpr float* calculate_slices()
{
	const float cascade_split_lambda = 0.95f;

	float* cascade_splits = new float[SHADOW_MAP_CASCADE_COUNT];

	const float near_clip = Z_NEAR;
	const float far_clip = Z_FAR;
	const float clip_range = far_clip - near_clip;

	const float ratio = far_clip / near_clip;

	/*
	  Calculate split depths based on view camera frustum
	  Based on method presented in
	  https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
	*/
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
	{
		float power = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
		float log = near_clip * std::pow(ratio, power);
		float uniform = near_clip + clip_range * power;
		float d = cascade_split_lambda * (log - uniform) + uniform;
		cascade_splits[i] = (d - near_clip) / clip_range;
	}
	return cascade_splits;
}

float* CascadeShadowSlice::cached_splits = calculate_slices();

/*
	Function is derived from Vulkan examples from Sascha Willems, and
	licensed under the MIT License :
	https ://github.com/SaschaWillems/Vulkan/tree/master/examples/shadowmappingcascade
	which are based on
	https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
*/
void CascadeShadowSlice::updateCascadeShadows(
	std::vector<CascadeShadowSlice>& shadows, const Scene& scene)
{
	glm::mat4 view = scene.camera.view_matrix;
	glm::mat4 projection = scene.projection.projection_matrix;
	glm::vec3 light_direction = scene.scene_lights.directional_light.direction;

	const float near_clip = Z_NEAR;
	const float far_clip = Z_FAR;
	const float clip_range = far_clip - near_clip;

	// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.0;
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float splitDist = cached_splits[i];

		glm::vec3 frustrum_corners[8] =
		{
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3(1.0f,  1.0f,  1.0f),
			glm::vec3(1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
		};

		// Project frustum corners into world space
		glm::mat4 inverse_camera = glm::inverse(projection * view);
		for (int i = 0; i < 8; ++i)
		{
			glm::vec4 invCorner = inverse_camera * 
				glm::vec4(frustrum_corners[i], 1.0f);
			frustrum_corners[i] = invCorner / invCorner.w;
		}

		for (int i = 0; i < 4; ++i)
		{
			glm::vec3 dist = frustrum_corners[i + 4] - frustrum_corners[i];
			frustrum_corners[i + 4] = frustrum_corners[i] + (dist * splitDist);
			frustrum_corners[i] = frustrum_corners[i] + (dist * lastSplitDist);
		}

		// Get frustum center
		glm::vec3 frustrum_center = glm::vec3(0.0f);
		for (int i = 0; i < 8; ++i)
		{
			frustrum_center += frustrum_corners[i];
		}
		frustrum_center /= 8.0f;

		float radius = 0.0f;
		for (int i = 0; i < 8; ++i)
		{
			float distance =
				glm::length(frustrum_corners[i] - frustrum_center);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		glm::mat4 lightViewMatrix = glm::lookAt(
			frustrum_center - light_direction * -minExtents.z, frustrum_center,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightOrthoMatrix = glm::ortho(
			minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f,
			maxExtents.z - minExtents.z);

		// Store split distance and matrix in cascade
		shadows[i].split_distance = (Z_NEAR + splitDist * clip_range) * -1.0f;
		shadows[i].projection_view_matrix = lightOrthoMatrix * lightViewMatrix;

		lastSplitDist = cached_splits[i];
	}
}