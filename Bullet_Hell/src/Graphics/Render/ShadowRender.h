#pragma once

#include "Globals.h"

#include "CascadeShadowSlice.h"
#include "ShadowBuffer.h"

#include <memory>

#include "glad.h"

/// <summary>
/// Handles rendering for shadows.
/// </summary>
class ShadowRender
{
public:
	CascadeShadowSlice cascade_shadows[SHADOW_MAP_CASCADE_COUNT];
	std::unique_ptr<ShadowBuffer> shadow_buffer;
};
