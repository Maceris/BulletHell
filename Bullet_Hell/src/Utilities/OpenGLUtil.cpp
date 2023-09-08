#include "OpenGLUtil.h"
#include "Globals.h"

#include <iostream>

bool OpenGLUtil::check_shader_compiled(GLuint shader_ID) {
    GLint is_compiled = 0;
    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE)
    {
#ifdef DEBUG
        GLint max_length = 0;
        glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &max_length);

        std::string log;
        log.reserve(max_length);

        glGetShaderInfoLog(shader_ID, max_length, &max_length, &log[0]);

        std::cout << "Shader " << shader_ID << " failed to compile: ";
        std::cout << log << std::endl;
#endif // DEBUG
        glDeleteShader(shader_ID);// Don't leak the shader.
        return false;
    }
    return true;
}

bool OpenGLUtil::check_shader_program_linked(GLuint program_ID) {
    GLint is_compiled = 0;
    glGetProgramiv(program_ID, GL_LINK_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE)
    {
#ifdef DEBUG
        GLint max_length = 0;
        glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &max_length);

        std::string log;
        log.reserve(max_length);

        glGetProgramInfoLog(program_ID, max_length, &max_length, &log[0]);

        std::cout << "Program " << program_ID << " failed to link: ";
        std::cout << log << std::endl;
#endif // DEBUG
        glDeleteProgram(program_ID);// Don't leak the program.
        return false;
    }
    return true;
}

bool OpenGLUtil::check_gl_errors()
{
    GLenum error = glGetError();
    bool found_error = false;
    while (error != GL_NO_ERROR) {
        found_error = true;
#ifdef DEBUG
        switch (error)
        {
        case GL_INVALID_ENUM:
            std::cout << "Error: An unacceptable value was specified for an enumerated argument.";
            break;
        case GL_INVALID_VALUE:
            std::cout << "Error: A numeric argument is out of range.";
            break;
        case GL_INVALID_OPERATION:
            std::cout << "Error: The specified operation is not allowed in the current state.";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "Error: The framebuffer object is not complete.";
            break;
        case GL_OUT_OF_MEMORY:
            std::cout << "Error: There is not enough memory left to execute the command.";
            break;
        case GL_STACK_UNDERFLOW:
            std::cout << "Error: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
            break;
        case GL_STACK_OVERFLOW:
            std::cout << "Error: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
            break;
        default:
            std::cout << "Error: An unknown error " << error << " has occurred.";
            break;
        }
        std::cout << std::endl;
#endif // DEBUG
        // Fetch a new error, if it exists
        error = glGetError();
    }
    return found_error;
}
