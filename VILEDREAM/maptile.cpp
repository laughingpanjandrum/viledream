#include "maptile.h"

string getMaptileName(const maptile tl)
{
	switch (tl)
	{
	case(TILE_ASH):				return "ash";
	case(TILE_BONES):			return "bones";
	case(TILE_BUSH):			return "bush";
	case(TILE_CARPET_RED):		return "carpet";
	case(TILE_CELL_WINDOW):		return "cell window";
	case(TILE_DIRT):			return "dirt floor";
	case(TILE_DOOR_CELL):		return "cell door";
	case(TILE_DOOR_WOODEN):		return "wooden door";
	case(TILE_FIRE_EVERBURNING):return "everburning fire";
	case(TILE_FLOOR):			return "floor";
	case(TILE_GRASS):			return "grass";
	case(TILE_GRAVE):			return "tombstone";
	case(TILE_MERCHANT):		return "merchant";
	case(TILE_MOSS):			return "moss";
	case(TILE_SAND):			return "sand";
	case(TILE_STASH_EQUIPMENT):	return "equipment stash";
	case(TILE_STATUE_SANDSTONE):return "sandstone statue";
	case(TILE_STATUE_STONE):	return "stone statue";
	case(TILE_TREE):			return "tree";
	case(TILE_TREE_DEAD):		return "dead tree";
	case(TILE_WALL):			return "wall";
	case(TILE_WALL_SANDSTONE):	return "sandstone wall";
	case(TILE_WALL_SUPER):		return "superwall";
	case(TILE_WATER_DEEP):		return "deep water";
	case(TILE_WATER_PUTRID):	return "putrid water";
	case(TILE_WOODEN_TABLE):	return "wooden table";
	default:
		return "maptile " + to_string(tl);
	}
}

int getMaptileGlyph(const maptile tl)
{
	switch (tl)
	{
	case(TILE_ASH):
		return ',';

	case(TILE_BONES):
		return '%';

	case(TILE_BUSH):
		return ';';

	case(TILE_DOOR_CELL):
	case(TILE_DOOR_WOODEN):
		return '+';

	case(TILE_CELL_WINDOW):
		return '=';

	case(TILE_DIRT):
	case(TILE_FLOOR):
	case(TILE_GRASS):
	case(TILE_SAND):
		return 249;

	case(TILE_CARPET_RED):
		return ':';

	case(TILE_FIRE_EVERBURNING):
		return '^';

	case(TILE_GRAVE):
		return 239;

	case(TILE_MERCHANT):
		return '@';

	case(TILE_MOSS):
		return '"';

	case(TILE_STASH_EQUIPMENT):
		return 220;

	case(TILE_STATUE_SANDSTONE):
	case(TILE_STATUE_STONE):
		return 225;

	case(TILE_TREE):
	case(TILE_TREE_DEAD):
		return 5;

	case(TILE_WALL):
	case(TILE_WALL_SANDSTONE):
	case(TILE_WALL_SUPER):
		return '#';

	case(TILE_WATER_DEEP):
	case(TILE_WATER_PUTRID):
		return 247;

	case(TILE_WOODEN_TABLE):
		return 194;

	default:
		return '?';
	}
}

colorType getMaptileColor(const maptile tl)
{
	switch (tl)
	{
	case(TILE_BUSH):
	case(TILE_GRASS):
	case(TILE_MOSS):
		return TCODColor::green;

	case(TILE_BONES):
		return TCODColor::darkYellow;

	case(TILE_TREE):
		return TCODColor::sepia;

	case(TILE_DIRT):
		return TCODColor::darkSepia;

	case(TILE_DOOR_CELL):
	case(TILE_CELL_WINDOW):
		return TCODColor::lightGrey;

	case(TILE_FLOOR):
	case(TILE_GRAVE):
	case(TILE_WALL):
		return TCODColor::grey;

	case(TILE_ASH):
	case(TILE_WALL_SUPER):
		return TCODColor::darkGrey;

	case(TILE_STASH_EQUIPMENT):
		return TCODColor::lightSepia;

	case(TILE_STATUE_STONE):
		return TCODColor::lightGrey;

	case(TILE_WATER_DEEP):
		return TCODColor::darkBlue;

	case(TILE_WATER_PUTRID):
		return TCODColor::lightSepia;

	case(TILE_CARPET_RED):
		return TCODColor::darkRed;
		
	case(TILE_DOOR_WOODEN):
	case(TILE_WOODEN_TABLE):
		return TCODColor::sepia;

	case(TILE_FIRE_EVERBURNING):
		return TCODColor::flame;

	case(TILE_SAND):
	case(TILE_WALL_SANDSTONE):
		return TCODColor::amber;

	case(TILE_STATUE_SANDSTONE):
	case(TILE_TREE_DEAD):
		return TCODColor::darkAmber;

	case(TILE_MERCHANT):
		return TCODColor::gold;

	default:
		return COLOR_WHITE;
	}
}

//	aesthetic feature
bool doesMaptileFlicker(const maptile tl)
{
	return tl == TILE_FIRE_EVERBURNING || tl == TILE_WATER_DEEP || tl == TILE_WATER_PUTRID;
}

bool isMaptileWalkable(const maptile tl)
{
	switch (tl)
	{
	case(TILE_CELL_WINDOW):
	case(TILE_GRAVE):
	case(TILE_MERCHANT):
	case(TILE_STASH_EQUIPMENT):
	case(TILE_STATUE_SANDSTONE):
	case(TILE_STATUE_STONE):
	case(TILE_TREE):
	case(TILE_TREE_DEAD):
	case(TILE_WALL):
	case(TILE_WALL_SANDSTONE):
	case(TILE_WALL_SUPER):
	case(TILE_WOODEN_TABLE):
		return false;

	default:
		return true;
	}
}

bool isMaptileTransparent(const maptile tl)
{
	switch (tl)
	{
	case(TILE_BUSH):
	case(TILE_DOOR_CELL):
	case(TILE_DOOR_WOODEN):
	case(TILE_WALL):
	case(TILE_WALL_SANDSTONE):
	case(TILE_WALL_SUPER):
		return false;

	default:
		return true;
	}
}

//	Returns true if the given tile can have a surface placed on it.
bool canCoverWithSurface(const maptile tl, const surface sf)
{
	if (tl == TILE_WATER_DEEP)
		return sf == SURFACE_ICE || sf == SURFACE_OIL;
	else
		return tl == TILE_ASH || tl == TILE_BONES || tl == TILE_DIRT || tl == TILE_FLOOR || tl == TILE_SAND || tl == TILE_WATER_DEEP || tl == TILE_WATER_PUTRID;
}



bool isMaptileFlammable(const maptile tl)
{
	switch (tl)
	{
	case(TILE_BUSH):
	case(TILE_DOOR_WOODEN):
	case(TILE_GRASS):
	case(TILE_MOSS):
	case(TILE_TREE):
	case(TILE_TREE_DEAD):
	case(TILE_WOODEN_TABLE):
		return true;

	default:
		return false;
	}
}


string getSurfaceName(const surface sf)
{
	switch (sf)
	{
	case(SURFACE_BLOOD):		return "blood";
	case(SURFACE_BURNING_OIL):	return "burning oil";
	case(SURFACE_CORPSE):		return "corpse";
	case(SURFACE_ELECTRIC):		return "electrification";
	case(SURFACE_ICE):			return "ice";
	case(SURFACE_FIRE):			return "fire";
	case(SURFACE_FROST):		return "frost";
	case(SURFACE_OIL):			return "oil";
	case(SURFACE_WATER):		return "water pool";
	case(SURFACE_WATER_ELEC):	return "electrified water";
	default:
		return "surface " + to_string(sf);
	}
}

int getSurfaceGlyph(const surface sf)
{
	switch (sf)
	{
	case(SURFACE_BLOOD):		
	case(SURFACE_FROST):
		return 249;

	case(SURFACE_BURNING_OIL):
	case(SURFACE_FIRE):
		return '^';

	case(SURFACE_ELECTRIC):
		return '*';

	case(SURFACE_ICE):
		return '=';

	case(SURFACE_CORPSE):		
		return '%';

	case(SURFACE_OIL):
	case(SURFACE_WATER):
	case(SURFACE_WATER_ELEC):
		return '~';
	
	default:
		return '?';
	}
}

colorType getSurfaceColor(const surface sf)
{
	switch (sf)
	{
	case(SURFACE_BLOOD):
	case(SURFACE_CORPSE):
		return TCODColor::darkRed;

	case(SURFACE_BURNING_OIL):
		return TCODColor(244, 53, 43);

	case(SURFACE_ELECTRIC):
	case(SURFACE_WATER_ELEC):
		return TCODColor::lightYellow;

	case(SURFACE_ICE):
	case(SURFACE_FROST):
		return TCODColor::cyan;

	case(SURFACE_FIRE):
		return TCODColor::flame;

	case(SURFACE_OIL):
		return TCODColor::purple;

	case(SURFACE_WATER):
		return TCODColor::lightBlue;

	default:
		return COLOR_WHITE;
	}
}

bool isSurfaceFlammable(const surface sf)
{
	switch (sf)
	{
	case(SURFACE_CORPSE):
	case(SURFACE_OIL):
		return true;

	default:
		return false;
	}
}
