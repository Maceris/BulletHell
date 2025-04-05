#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "graphics/frontend/buffer.h"

#include "debugging/logger.h"

#include "glad.h"

Buffer::Buffer(Type type)
	: handle{0}
	, type{type}
{
	GLuint buffer_id = 0;
	switch (type)
	{
	case Type::UNIFORM:
		glGenBuffers(1, &buffer_id);
		break;
	case Type::SHADER_STORAGE:
		glGenBuffers(1, &buffer_id);
		break;
	default:
		LOG_FATAL("Unknown buffer type");
	}
	handle = static_cast<BufferHandle>(buffer_id);
}

#endif