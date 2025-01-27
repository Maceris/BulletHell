#pragma once

#include <memory>

struct Entity;
struct MeshData;
struct Model;

constexpr auto SKYBOX_VBO_COUNT = 3;

class SkyBox
{
public:
	SkyBox();
	SkyBox(const SkyBox&) = delete;
	SkyBox& operator=(const SkyBox&) = delete;
	~SkyBox() = default;

	std::shared_ptr<Entity> entity;
	std::shared_ptr<Model> model;
};