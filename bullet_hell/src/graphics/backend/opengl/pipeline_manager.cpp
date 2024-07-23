#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL || BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/backend/base/pipeline_manager.h"

std::unique_ptr<Texture> PipelineManager::default_texture = nullptr;

#endif
