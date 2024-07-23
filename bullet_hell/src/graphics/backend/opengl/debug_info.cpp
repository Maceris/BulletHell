#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/base/debug_info.h"

#include "glad.h"

#if 0 //TODO(ches) replace the old debug render with this

struct LineGroup::Data
{
    Data()
        : vao{ 0 }
        , data{ 0 }
        , count{ 0 }
    {}
    Data(const Data& data) = delete;
    Data& operator=(const Data& data) = delete;
    ~Data() = default;

    /// <summary>
    /// The VAO for this group.
    /// </summary>
    GLuint vao;
    /// <summary>
    /// The buffer handle for the line data.
    /// </summary>
    GLuint data;
    /// <summary>
    /// The number of lines. Notably, not the number of points like 
    /// glDrawArrays expects.
    /// </summary>
    int count;
};

LineGroup::LineGroup()
    : data{}
{
    glGenVertexArrays(1, &data->vao);
    glGenBuffers(1, &data->data);
    glBindVertexArray(data->vao);
    glBindBuffer(GL_ARRAY_BUFFER, data->data);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (const void*)0);
    glBindVertexArray(0);
}

LineGroup::~LineGroup()
{
    glDeleteBuffers(1, &data->data);
    glDeleteVertexArrays(1, &data->vao);
}

Line Line::operator+(const glm::vec3& displacement) const
{
    const glm::vec4 displace_vec4(displacement, 0.0f);
    return Line(start + displace_vec4, end + displace_vec4);
}

AABBLines::AABBLines(const glm::vec3& aabb_min, const glm::vec3& aabb_max)
{
    //NOTE(ches) min corner
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z)
    );

    //NOTE(ches) max corner
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z),
        glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    );

    //NOTE(ches) The rest of the lines
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    );
}
#endif

#endif
