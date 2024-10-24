#pragma once

#include <cstdint>

#include "utilities/generic_iterator.h"

class Scene;

class RenderStage
{
public:
	enum class Type : uint8_t;
    
    virtual ~RenderStage() = default;

	virtual void render(Scene& scene) = 0;
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

using RenderStageTypeIterator = Iterator<RenderStage::Type,
    RenderStage::Type::ANIMATION, RenderStage::Type::SKYBOX>;

/// <summary>
/// A pointer to some resource that is used in render stages, preferably
/// shared between stages.
/// 
/// Stages hold pointer to these (pointers), so when
/// resources are updated, all the render stages have the latest resource.
/// </summary>
/// <typeparam name="T">The type of resource.</typeparam>
template <typename T>
using StageResource = T*;
