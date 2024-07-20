#pragma once

#include <memory>

#include "graphics/frontend/texture.h"

class PipelineManager
{
public:
	static std::unique_ptr<Texture> default_texture;
};