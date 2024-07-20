#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/buffer.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/shader.h"
#include "graphics/graph/cascade_shadow_slice.h"

class LightRender : RenderStage
{
public:
	LightRender(Shader* shader,
		CascadeShadows* cascade_shadows,
		Buffer point_lights,
		Buffer spot_lights,
		Framebuffer* shadow_buffers,
		Framebuffer* gbuffer,
		const QuadMesh* quad_mesh)
		: shader{ shader }
		, cascade_shadows{ cascade_shadows }
		, point_lights{ point_lights }
		, spot_lights{ spot_lights }
		, shadow_buffers{ shadow_buffers }
		, gbuffer{ gbuffer }
		, quad_mesh{ quad_mesh }
	{}

	void render(Scene& scene);

private:
	const Shader* shader;
	const CascadeShadows* cascade_shadows;
	const Buffer point_lights;
	const Buffer spot_lights;
	const Framebuffer* shadow_buffers;
	const Framebuffer* gbuffer;
	const QuadMesh* quad_mesh;
};