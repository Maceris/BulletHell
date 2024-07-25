#pragma once

#include <cstdint>

#include "utilities/generic_iterator.h"

class Scene;

class RenderStage
{
public:
	enum class Type : uint8_t;

	void render(Scene& scene);
};

enum class RenderStage::Type : uint8_t
{
    ANIMATION,
    DEBUG_INFO,
    FILTER,
    GUI,
    LIGHT,
    SCENE,
    SHADOW,
    SKYBOX
};

using RenderStageTypeIterator = Iterator<RenderStage::Type, RenderStage::Type::ANIMATION, RenderStage::Type::SKYBOX>;
