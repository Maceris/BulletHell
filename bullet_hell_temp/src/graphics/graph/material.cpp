#include "Graphics/Graph/Material.h"

#include <cmath>

bool floats_equal(const float& a, const float& b) noexcept
{
	return std::abs(a - b) < 0.0001f;
}

bool colors_equal(const glm::vec4 color1, const glm::vec4 color2) noexcept
{
	return floats_equal(color1.r, color2.r)
		&& floats_equal(color1.g, color2.g)
		&& floats_equal(color1.b, color2.b)
		&& floats_equal(color1.a, color2.a);
}

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
	seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

bool Material::operator==(const Material& other) const noexcept
{
	//NOTE(ches) right now we ignore ambient color
	return texture_name == other.texture_name
		&& normal_map_name == other.normal_map_name
		&& floats_equal(reflectance, other.reflectance)
		&& colors_equal(diffuse_color, other.diffuse_color)
		&& colors_equal(specular_color, other.specular_color);
}

std::size_t MaterialHash::operator()(const Material& material) const noexcept
{
	std::size_t hash = 0;
	hash_combine(hash, material.texture_name);
	hash_combine(hash, material.normal_map_name);
	hash_combine(hash, material.reflectance);
	hash_combine(hash, material.diffuse_color.r);
	hash_combine(hash, material.diffuse_color.g);
	hash_combine(hash, material.diffuse_color.b);
	hash_combine(hash, material.specular_color.r);
	hash_combine(hash, material.specular_color.g);
	hash_combine(hash, material.specular_color.b);
	return hash;
}
