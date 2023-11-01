#pragma once

#include "Globals.h"

#include <cstdint>

/// <summary>
/// The type of tile.
/// </summary>
using TileID = uint8_t;

/// <summary>
/// Represents a tile in the world map, something like ground, walls, 
/// a pillar.
/// </summary>
struct Tile
{
	/// <summary>
	/// The type of tile this is.
	/// </summary>
	TileID id;

	Tile();
	Tile(const TileID id);
	Tile(const Tile&) = default;
	Tile& operator=(const Tile&) = default;
	~Tile() = default;
};

#pragma region Tile ID constants
constexpr TileID TILE_VOID = 0;
constexpr TileID TILE_GROUND = 1;
#pragma endregion
