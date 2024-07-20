#pragma once

#include <cstdint>

using TextureHandle = uint64_t;

struct Texture
{
	const TextureHandle handle;
	const uint32_t width;
	const uint32_t height;

	/// <summary>
	/// Load a texture from a buffer.
	/// </summary>
	/// <param name="buffer">The texture data to load from.</param>
	/// <param name="width">The width of the texture in pixels.</param>
	/// <param name="height">The height of the texture in pixels.</param>
	Texture(const TextureHandle handle, const uint32_t width,
		const uint32_t height)
		: handle{ handle }
		, width{ width }
		, height{ height }
	{}

	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;
	~Texture() = default;

	/// <summary>
	/// Convenience method for casting the handle to a particular type.
	/// </summary>
	/// <typeparam name="T">The type to convert to.</typeparam>
	/// <returns>The handle, cast to the specified type.</returns>
	template <typename T>
		requires (sizeof(T) >= sizeof(TextureHandle))
	constexpr T get_handle() const
	{
		return static_cast<T>(handle);
	}
};
