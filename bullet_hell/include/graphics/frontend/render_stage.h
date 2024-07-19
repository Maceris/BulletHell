#pragma once

#include <cstdint>

class Scene;

class RenderStage
{
public:
	enum class Type;

	void render(Scene& scene);
};

enum class RenderStage::Type : uint8_t
{
    ANIMATION,
    DEBUG,
    FILTER,
    GUI,
    LIGHT,
    SCENE,
    SHADOW,
    SKYBOX
};
