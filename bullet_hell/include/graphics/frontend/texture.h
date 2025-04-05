#pragma once

#include <cstdint>

using TextureHandle = uint64_t;
using MemoryHandle = uint64_t;

struct Texture
{
	const TextureHandle handle;
	const MemoryHandle memory_handle;
	const uint32_t width;
	const uint32_t height;

	Texture(const TextureHandle handle, const MemoryHandle memory_handle, 
		const uint32_t width, const uint32_t height)
		: handle{ handle }
		, memory_handle{ handle }
		, width{ width }
		, height{ height }
	{}

	Texture(const TextureHandle handle, const uint32_t width,
		const uint32_t height)
		: Texture{handle, 0, width, height}
	{}

	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;

	/// <summary>
	/// Does nothing important, the expectation is the backing resources are 
	/// cleaned up via the deletion queue.
	/// </summary>
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
