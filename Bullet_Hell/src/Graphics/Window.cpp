#include "Window.h"

#include "Globals.h"
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
#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4x4.hpp"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


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
    }
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

    const char* glsl_version = "#version 460";
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

    handle = glfwCreateWindow(width, height, "Title TBD", NULL, NULL);
    if (!handle)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(handle, key_callback);

    glfwMakeContextCurrent(handle);
    gladLoadGL();
    glfwSwapInterval(1);

#ifdef DEBUG
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
#endif // DEBUG

    OpenGLUtil::check_gl_errors();

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    OpenGLUtil::check_gl_errors();
    OpenGLUtil::check_shader_compiled(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    OpenGLUtil::check_gl_errors();
    OpenGLUtil::check_shader_compiled(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    OpenGLUtil::check_gl_errors();
    OpenGLUtil::check_shader_program_linked(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));
}

bool Window::should_close()
{
    return glfwWindowShouldClose(handle);
}

void Window::render()
{
    float ratio;
    int width, height;
    glm::fmat4x4 m, screen_projection, transformed;

    glfwGetFramebufferSize(handle, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    m = glm::mat4(1.0);
    glm::rotate(m, (float)glfwGetTime(), glm::vec3(0, 0, 1));
    screen_projection = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    transformed = m * screen_projection;

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(transformed));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(handle);
    glfwPollEvents();
}

void Window::terminate()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}