#pragma once

#include "Globals.h"

#include "glad.h"
#include "glfw3.h"

#include "AnimationRender.h"
#include "FilterRender.h"
#include "GuiRender.h"
#include "LightRender.h"
#include "SceneRender.h"
#include "ShadowRender.h"
#include "SkyBoxRender.h"
#include "Window.h"

#include "GBuffer.h"

// Handles all the rendering stages for drawing to the screen
class Render
{
public:
	// Set up a rendering pipeline for the given window
	Render(const Window& window);
private:
	AnimationRender animation_render;
	GBuffer gBuffer;
	GuiRender gui_render;
	LightRender light_render;

};
