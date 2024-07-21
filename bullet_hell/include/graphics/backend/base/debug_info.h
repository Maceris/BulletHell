#pragma once

#include <memory>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "graphics/glad_types.h"

/// <summary>
/// A set of lines to draw.
/// </summary>
struct LineGroup
{
	friend class DebugRender;

	LineGroup();
	LineGroup(const LineGroup&) = delete;
	LineGroup& operator=(const LineGroup&) = delete;
	~LineGroup();
private:
    struct Data;
    const std::unique_ptr<Data> data;
};

struct DebugInfo
{
    DebugInfo() = default;
    DebugInfo(const DebugInfo&) = delete;
    DebugInfo& operator=(const DebugInfo&) = delete;
    ~DebugInfo() = default;

    LineGroup map_lines;
    LineGroup hot_chunk_lines;
    LineGroup cold_chunk_lines;
    LineGroup frustum_lines;
    LineGroup AABB_lines;
};

#pragma pack(push,1)
struct Line
{
	glm::vec4 start;
	glm::vec4 end;

	Line(const glm::vec3& start, const glm::vec3& end)
		: start{ start, 1.0f }
		, end{ end, 1.0f }
	{}
	Line(const Line&) = default;
	Line& operator=(const Line&) = default;
	~Line() = default;

	Line operator+(const glm::vec3& displacement) const;
};
#pragma pack(pop)

struct AABBLines
{
    std::vector<Line> lines;

	AABBLines(const glm::vec3& aabb_min, const glm::vec3& aabb_max);
    AABBLines(const AABBLines&) = default;
    AABBLines& operator=(const AABBLines&) = default;
    ~AABBLines() = default;
};
