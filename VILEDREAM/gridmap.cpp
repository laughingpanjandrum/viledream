#include "gridmap.h"



gridmap::gridmap(const int xsize, const int ysize) :
	_xsize(xsize), _ysize(ysize)
{
	//	empty lists of tiles/surfaces/etc
	for (unsigned x = 0; x < _xsize; x++)
	{
		vector<maptile> xtiles;
		vector<surface> xsurf;
		vector<itemPtr> xitems;
		vector<bool> xmem;
		for (unsigned y = 0; y < _ysize; y++)
		{
			xtiles.push_back(TILE_FLOOR);
			xsurf.push_back(SURFACE__NONE);
			xitems.push_back(nullptr);
			xmem.push_back(false);
		}
		_tiles.push_back(xtiles);
		_surfaces.push_back(xsurf);
		_items.push_back(xitems);
		_memoryMap.push_back(xmem);
	}

	//	this map is used for fov/pathfinding
	_tmap = TCODMapPtr(new TCODMap(_xsize, _ysize));
}


void gridmap::addCreature(creaturePtr cr, const intpair pt)
{
	_creatures.push_back(cr);
	cr->_pos = pt;
}

void gridmap::removeCreature(creaturePtr cr)
{
	auto f = find(_creatures.begin(), _creatures.end(), cr);
	if (f != _creatures.end())
		_creatures.erase(f);
}

//	Returns the creature at the given point, if any.
//	May have to cycle through EVERY creature, so don't call it a lot!
creaturePtr gridmap::getCreature(const intpair pt) const
{
	for (auto cr : _creatures)
	{
		if (cr->_pos == pt)
			return cr;
	}
	return nullptr;
}

//	places the item on the given square, unless it's occupied;
//	in which case try to find a non-occupied square adjacent.
//	if there are NO available squares, the item is not dropped.
//	will also try to stack with nearby existing items, if possible.
void gridmap::addItemNear(itemPtr it, const intpair pt)
{
	//	target square is clear
	auto here = getItem(pt);

	//	is there an item here already?
	if (here == nullptr)
		addItem(it, pt);

	//	can we stack with it?
	else if (here->stacksWith(it.get()))
		here->_amount += it->_amount;

	//	check adjacent squares
	else
	{
		for (int x = pt.first - 1; x <= pt.first + 1; x++)
		{
			for (int y = pt.second - 1; y <= pt.second + 1; y++)
			{
				//	same checks as above with items on these squares
				here = getItem(x, y);
				if (here == nullptr)
				{
					addItem(it, x, y);
					return;
				}
				else if (here->stacksWith(it.get()))
				{
					here->_amount += it->_amount;
					return;
				}
			}
		}
	}
}

//	Create a new flight of stairs at the given position.
void gridmap::addStairs(const intpair pos, string id, string name, int dl, int glyph)
{
	_stairs.push_back(mapStairsPtr(new MapStairs(name, id, pos, dl, glyph)));
}


//	Special stairs that lead to our home base
void gridmap::addStairsToHomeBase(const intpair pos)
{
	addStairs(pos, "fireholm", "stairs to Fireholm", 0);
}


//	Returns true if there are stairs at the given coordinate.
bool gridmap::hasStairs(const intpair pt) const
{
	return getStairs(pt) != nullptr;
}

mapStairsPtr gridmap::getStairs(const intpair pt) const
{
	for (auto st : _stairs)
	{
		if (st->_pos == pt)
			return st;
	}
	return nullptr;
}

void gridmap::setFOVMapCentre(const intpair ctr)
{
	_tmap->computeFov(ctr.first, ctr.second, 20);
}

void gridmap::updateTmapAtPoint(const int x, const int y)
{
	_tmap->setProperties(x, y, isTransparent(x, y), isWalkable(x, y));
}

void gridmap::updateTmap()
{
	for (unsigned x = 0; x < _xsize; x++)
	{
		for (unsigned y = 0; y < _ysize; y++)
			updateTmapAtPoint(x, y);
	}
}


//	due to a flammable surface OR tile
bool gridmap::willFireSpreadToPoint(const int x, const int y) const
{
	return isSurfaceFlammable(getSurface(x, y)) || isMaptileFlammable(getTile(x, y));
}


void gridmap::tickSurfaces()
{
	//	Fires spread/burn out
	vector<intpair> new_fires;
	for (unsigned x = 0; x < _xsize; x++)
	{
		for (unsigned y = 0; y < _ysize; y++)
		{
			auto sf = getSurface(x, y);

			//	Fires burn away
			if (sf == SURFACE_FIRE || sf == SURFACE_BURNING_OIL)
			{
				//	spread to adjacent squares
				for (int nx = x - 1; nx <= x + 1; nx++)
				{
					for (int ny = y - 1; ny < y + 1; ny++)
					{
						if (!(nx == x && ny == y) && inBounds(nx, ny) && willFireSpreadToPoint(nx, ny))
							new_fires.push_back(intpair(nx, ny));
					}
				}

				//	check whether this fire burns out
				bool burn_out = (sf == SURFACE_FIRE && roll_one_in(3)) || (sf == SURFACE_BURNING_OIL && roll_one_in(6));
				if (burn_out)
					_surfaces[x][y] = SURFACE__NONE;
			}

			//	Electric tiles eventually become non-electric
			else if (sf == SURFACE_ELECTRIC || sf == SURFACE_WATER_ELEC)
			{
				bool die_out = (sf == SURFACE_ELECTRIC && roll_one_in(3)) || (sf == SURFACE_WATER_ELEC && roll_one_in(6));
				if (die_out)
					_surfaces[x][y] = SURFACE__NONE;
			}
		}
	}

	//	Place new fires
	for (auto pt : new_fires)
		setSurface(SURFACE_FIRE, pt);
}
