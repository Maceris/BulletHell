#include "Window.h"

#include "Globals.h"

#include "GameLogic.h"
#include "Logger.h"
#include "OpenGLUtil.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include "glm.hpp"
#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

/// <summary>
/// The callback to register for handling errors.
/// </summary>
/// <param name="error">The error code.</param>
/// <param name="description">The text description of the error.</param>
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

/// <summary>
/// The callback to register for handling key presses.
/// </summary>
/// <param name="window">The window that received the event.</param>
/// <param name="key">The keyboard key that was pressed or released.</param>
/// <param name="scancode">The system-specific scancode of the key.</param>
/// <param name="action">GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.</param>
/// <param name="mods">Bit field describing which modifier keys were
/// held down.</param>
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        g_game_logic->request_close();
    }
}

/// <summary>
/// A callback to register for when the window gets resized.
/// </summary>
/// <param name="window">The window that was resized.</param>
/// <param name="width">The new width, in pixels, of the framebuffer.</param>
/// <param name="height">The new height, in pixels, of the framebuffer.</param>
static void resize_callback(GLFWwindow* window, int width, int height)
{
    g_game_logic->notify_about_resize(width, height);
}

void Window::initialize()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    if (options.anti_aliasing)
    {
        glfwWindowHint(GLFW_SAMPLES, 4);
    }

#if defined(IMGUI_IMPL_OPENGL_ES2)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    if (options.compatible_profile)
    {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    }
    else
    {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    }

    if (options.width > 0 || options.height > 0)
    {
        width = options.width;
        height = options.height;
    }
    else
    {
        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
        const GLFWvidmode* video_mode 
            = glfwGetVideoMode(glfwGetPrimaryMonitor());
        width = video_mode->width;
        height = video_mode->height;
    }

    handle = glfwCreateWindow(width, height, 
        g_game_logic->options.window_title.c_str(), nullptr, nullptr);
    if (!handle)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(handle, key_callback);
    glfwSetFramebufferSizeCallback(handle, resize_callback);

    glfwMakeContextCurrent(handle);
    gladLoadGL();
    glfwSwapInterval(1);

    LOG_INFO("Initialized window with opengl v" 
        + std::string((char*)glGetString(GL_VERSION)));
#if 0
#if DEBUG
    OpenGLUtil::log_extensions();
#endif // DEBUG
#endif

    std::vector<std::string> required_extensions;
    required_extensions.emplace_back("GL_ARB_compute_shader");
    required_extensions.emplace_back("GL_ARB_compute_variable_group_size");
    required_extensions.emplace_back("GL_ARB_fragment_shader");
    required_extensions.emplace_back("GL_ARB_framebuffer_object");
    required_extensions.emplace_back("GL_ARB_multi_draw_indirect");
    required_extensions.emplace_back("GL_ARB_separate_shader_objects");
    required_extensions.emplace_back("GL_ARB_shader_storage_buffer_object");
    required_extensions.emplace_back("GL_ARB_vertex_array_object");
    required_extensions.emplace_back("GL_ARB_vertex_buffer_object");
    required_extensions.emplace_back("GL_ARB_vertex_shader");

    OpenGLUtil::assert_extensions_exist(required_extensions);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(handle);
}

void Window::render()
{
    glfwSwapBuffers(handle);
    glfwPollEvents();
}

void Window::terminate()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}