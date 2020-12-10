#pragma once

#include <string>
#include <vector>

#include "color_constants.h"

using namespace std;


//	Base map tiles

enum maptile
{
	TILE_ASH,
	TILE_BONES,
	TILE_BUSH,
	TILE_CARPET_RED,
	TILE_CELL_WINDOW,
	TILE_DIRT,
	TILE_DOOR_CELL, TILE_DOOR_WOODEN,
	TILE_FIRE_EVERBURNING,
	TILE_FLOOR,
	TILE_GRASS,
	TILE_GRAVE,
	TILE_MOSS,
	TILE_SAND,
	TILE_MERCHANT,
	TILE_STASH_EQUIPMENT,
	TILE_STATUE_SANDSTONE, TILE_STATUE_STONE,
	TILE_TREE, TILE_TREE_DEAD,
	TILE_WALL, TILE_WALL_SANDSTONE,
	TILE_WALL_SUPER,
	TILE_WATER_DEEP, TILE_WATER_PUTRID,
	TILE_WOODEN_TABLE,
};

string getMaptileName(const maptile tl);
int getMaptileGlyph(const maptile tl);
colorType getMaptileColor(const maptile tl);
bool doesMaptileFlicker(const maptile tl);


bool isMaptileWalkable(const maptile tl);
bool isMaptileTransparent(const maptile tl);
bool isMaptileFlammable(const maptile tl);



//	Surfaces

enum surface
{
	SURFACE_BLOOD, 
	SURFACE_BURNING_OIL,
	SURFACE_CORPSE,
	SURFACE_ELECTRIC,
	SURFACE_FIRE,
	SURFACE_FROST,
	SURFACE_ICE,
	SURFACE_OIL,
	SURFACE_WATER,
	SURFACE_WATER_ELEC,
	SURFACE__NONE,
};

string getSurfaceName(const surface sf);
int getSurfaceGlyph(const surface sf);
colorType getSurfaceColor(const surface sf);

bool isSurfaceFlammable(const surface sf);

bool canCoverWithSurface(const maptile tl, const surface sf);