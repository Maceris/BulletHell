#pragma once

#include "Globals.h"

#include <compare>

#include "Tile.h"

/// <summary>
/// The number of tiles per side of a chunk.
/// </summary>
constexpr int CHUNK_WIDTH = 16;

/// <summary>
/// The location in the world, which can also be referenced as a combined
/// value.
/// </summary>
union ChunkCoordinates
{
	/// <summary>
	/// The x and z coordinates as separate values.
	/// </summary>
	struct
	{
		/// <summary>
		/// The x coordinate of the chunk, measured in chunks.
		/// </summary>
		int16_t x;

		/// <summary>
		/// The z coordinate of the chunk, measured in chunks.
		/// </summary>
		int16_t z;
	};

	/// <summary>
	/// The x and z coordiantes, combined into one integer, for use as a 
	/// pseudo-hash.
	/// </summary>
	uint32_t combined;

	ChunkCoordinates()
		: combined{ 0 }
	{}

	explicit ChunkCoordinates(const uint32_t combined)
		: combined{ combined }
	{}

	explicit ChunkCoordinates(const int16_t x, const int16_t z)
		: x{ x }
		, z{ z }
	{}

	explicit ChunkCoordinates(const int x, const int z)
		: x{ static_cast<int16_t>(x) }
		, z{ static_cast<int16_t>(z) }
	{}

	explicit ChunkCoordinates(const unsigned int x, const unsigned int z)
		: x{ static_cast<int16_t>(x) }
		, z{ static_cast<int16_t>(z) }
	{}

	ChunkCoordinates(const ChunkCoordinates& other)
		: combined{ other.combined }
	{}

	ChunkCoordinates& operator=(const ChunkCoordinates& other)
	{
		if (this != &other)
		{
			combined = other.combined;
		}
		return *this;
	}

	~ChunkCoordinates() = default;

	bool operator==(const ChunkCoordinates& other) const
	{
		return combined == other.combined;
	}

	auto operator<=>(const ChunkCoordinates& other) const
	{
		return combined <=> other.combined;
	}
};

/// <summary>
/// A cluster of tiles.
/// </summary>
struct Chunk
{
	/// <summary>
	/// The x and z coordinates of the chunk, measured in chunks.
	/// </summary>
	ChunkCoordinates location;

	/// <summary>
	/// The tiles that make up the chunk. They are stored with the first
	/// coordinate being the x axis, and the second being the z axis.
	/// 
	/// These tiles start at the lowest x and y coordinates, and go up towards
	/// the +x and +z axis.
	/// </summary>
	Tile tiles[CHUNK_WIDTH][CHUNK_WIDTH];

	Chunk();
	Chunk(const ChunkCoordinates& coordinates);
	Chunk(const Chunk&) = default;
	Chunk& operator=(const Chunk&) = default;
	~Chunk() = default;
};