#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/framebuffer_transition.h"

#include "glad.h"

void FramebufferTransition::render(Scene& scene)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (*framebuffer)->handle);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (*framebuffer)->width, (*framebuffer)->height);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(blend_s_factor, blend_d_factor);
}

#endif
