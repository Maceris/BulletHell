#include "Utilities/OpenGLUtil.h"

#include "Debugging/Logger.h"

#include "glad.h"

std::vector<std::string> OpenGLUtil::cached_extensions;

bool OpenGLUtil::assert_extensions_exist(const std::vector<std::string>& names)
{
    if (names.empty())
    {
        return true;
    }
    
    if (cached_extensions.empty())
    {
        int extension_count = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);

        for (int i = 0; i < extension_count; i++)
        {
            const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
            std::string name((const char*)extension);
            cached_extensions.push_back(name);
        }
    }

    bool found = false;
    for (const std::string& required_name : names)
    {
        found = false;
        for (const std::string& known_name : cached_extensions)
        {
            if (known_name == required_name)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            LOG_FATAL("Can not find the required OpenGL extension " 
                + required_name + ", this system may not be supported.");
        }
    }

    return true;
}

bool OpenGLUtil::check_gl_errors()
{
    GLenum error = glGetError();
    bool found_error = false;
    while (error != GL_NO_ERROR) {
        found_error = true;
        switch (error)
        {
        case GL_INVALID_ENUM:
            LOG_ERROR("Error: An unacceptable value was specified for an enumerated argument.");
            break;
        case GL_INVALID_VALUE:
            LOG_ERROR("Error: A numeric argument is out of range.");
            break;
        case GL_INVALID_OPERATION:
            LOG_ERROR("Error: The specified operation is not allowed in the current state.");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            LOG_ERROR("Error: The framebuffer object is not complete.");
            break;
        case GL_OUT_OF_MEMORY:
            LOG_ERROR("Error: There is not enough memory left to execute the command.");
            break;
        case GL_STACK_UNDERFLOW:
            LOG_ERROR("Error: An attempt has been made to perform an operation that would cause an internal stack to underflow.");
            break;
        case GL_STACK_OVERFLOW:
            LOG_ERROR("Error: An attempt has been made to perform an operation that would cause an internal stack to overflow.");
            break;
        default:
            LOG_ERROR("Error: An unknown error " + std::to_string(error)
                + " has occurred.");
            break;
        }
        // Fetch a new error, if it exists
        error = glGetError();
    }
    return found_error;
}

bool OpenGLUtil::check_shader_compiled(const GLuint& shader_ID) {
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

        LOG_ERROR("Shader " + std::to_string(shader_ID) 
            + " failed to compile: " + log);
#endif //DEBUG
        glDeleteShader(shader_ID);// Don't leak the shader.
        return false;
    }
    return true;
}

bool OpenGLUtil::check_shader_program_linked(const GLuint& program_ID) {
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

        LOG_ERROR("Program " + std::to_string(program_ID)
            + " failed to link. Error: '" + log + "'");
#endif //DEBUG
        glDeleteProgram(program_ID);// Don't leak the program.
        return false;
    }
    return true;
}

void OpenGLUtil::log_extensions()
{
    int extension_count = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
    
    LOG_INFO("All loaded OpenGL extensions ("
        + std::to_string(extension_count) + "):");

    for (int i = 0; i < extension_count; i++)
    {
        const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
        std::string name((const char*)extension);
        LOG_INFO(name);
    }
}

