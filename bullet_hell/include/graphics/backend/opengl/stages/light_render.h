#pragma once

#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/buffer.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/graph/cascade_shadow_slice.h"

class LightRender : public RenderStage
{
public:
	LightRender(StageResource<CascadeShadows>* cascade_shadows,
		StageResource<Buffer>* point_lights,
		StageResource<Buffer>* spot_lights,
		StageResource<Framebuffer>* shadow_buffers,
		StageResource<Framebuffer>* gbuffer,
		StageResource<QuadMesh>* quad_mesh);

	void render(Scene& scene);

private:
	Shader* shader;
	StageResource<CascadeShadows>* const cascade_shadows;
	StageResource<Buffer>* const point_lights;
	StageResource<Buffer>* const spot_lights;
	StageResource<Framebuffer>* const shadow_buffers;
	StageResource<Framebuffer>* const gbuffer;
	StageResource<QuadMesh>* const quad_mesh;

	void create_uniforms();
	void update_lights(const Scene& scene);
	void setup_point_light_buffer(const Scene& scene);
	void setup_spot_light_buffer(const Scene& scene);
};