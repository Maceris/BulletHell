#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "graphics/backend/opengl/quad_mesh.h"

QuadMesh::QuadMesh()
	: vao{ 0 }
	, vbo_list{ 0 }
{
	const float positions[] =
	{
		-1.0f,  1.0f, 0.0f, 
		 1.0f,  1.0f, 0.0f, 
		-1.0f, -1.0f, 0.0f, 
		 1.0f, -1.0f, 0.0f
	};
	const float texture_coordinates[] = 
	{
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	const int indices[] = {0, 2, 1, 1, 2, 3};
	
}

QuadMesh::~QuadMesh()
{

}
#endif
