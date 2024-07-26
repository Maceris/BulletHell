#include "graphics/window.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "graphics/graph/icon_resource.h"
#include "graphics/gui/ui.h"
#include "main/game_logic.h"
#include "utilities/opengl_util.h"

#include "glad.h"

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
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }
    g_game_logic->on_key_pressed(key, scancode, action, mods);
}

/// <summary>
/// A callback to register for when the window gets resized.
/// </summary>
/// <param name="window">The window that was resized.</param>
/// <param name="width">The new width, in pixels, of the framebuffer.</param>
/// <param name="height">The new height, in pixels, of the framebuffer.</param>
static void resize_callback(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0)
    {
        LOG_WARNING("Updating size with a width of " + std::to_string(width)
            + " and height of " + std::to_string(height));
        return;
    }
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

    glfwSetCursorPosCallback(handle, 
        [](GLFWwindow* window, double xpos, double ypos)
        {
            auto& pos = g_game_logic->window->mouse_input.current_position;
            pos.x = static_cast<float>(xpos);
            pos.y = static_cast<float>(ypos);
        }
    );
    glfwSetCursorEnterCallback(handle, 
        [](GLFWwindow* window, int entered)
        {
            g_game_logic->window->mouse_input.in_window = entered;
        }
    );
    glfwSetMouseButtonCallback(handle,
        [](GLFWwindow* window, int button, int action, int mods)
        {
            auto& input = g_game_logic->window->mouse_input;
            if (button == GLFW_MOUSE_BUTTON_1)
            {
                switch (action)
                {
                case GLFW_PRESS:
                    input.left_button_pressed = true;
                    break;
                case GLFW_RELEASE:
                    input.left_button_pressed = false;
                    break;
                case GLFW_REPEAT:
                default:
                    break;
                }
            }
            if (button == GLFW_MOUSE_BUTTON_2)
            {
                switch (action)
                {
                case GLFW_PRESS:
                    input.right_button_pressed = true;
                    glfwSetInputMode(window, GLFW_CURSOR, 
                        GLFW_CURSOR_DISABLED);
                    break;
                case GLFW_RELEASE:
                    input.right_button_pressed = false;
                    glfwSetInputMode(window, GLFW_CURSOR,
                        GLFW_CURSOR_NORMAL);
                    break;
                case GLFW_REPEAT:
                default:
                    break;
                }
            }
        }
    );

    glfwMakeContextCurrent(handle);
    gladLoadGL();
    glfwSwapInterval(1);

    set_window_icon();

    LOG_INFO("Initialized window with opengl v" 
        + std::string((char*)glGetString(GL_VERSION)));
#if 0
#if _DEBUG
    OpenGLUtil::log_extensions();
#endif // _DEBUG
#endif

#if 0
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
#endif
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

void Window::set_window_icon()
{
    auto icon = load_icon("textures/game_icon.image");
    GLFWimage images[1]{ icon->width, icon->height, icon->data };
    glfwSetWindowIcon(handle, 1, images);
}