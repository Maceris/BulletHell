#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/frontend/instance.h"

Instance::Instance()
{

}

Instance::~Instance()
{

}
void Instance::initialize(const Window& window)
{

}

void Instance::cleanup()
{

}

void Instance::process_resources()
{

}

void Instance::render(const Scene& scene)
{

}

void Instance::resize(int width, int height)
{

}

void Instance::setup_data(const Scene& scene)
{

}

void Instance::swap_pipeline(RenderConfig config)
{

}


#endif