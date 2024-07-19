#include "map/tile.h"

Tile::Tile()
	: id{ TILE_VOID }
{}

Tile::Tile(const TileID id)
	:id{ id }
{}
