#include "mapgen.h"



intpair mapgen::getRandomPoint(gridmapPtr m)
{
	int x = randrange(m->_xsize);
	int y = randrange(m->_ysize);
	return intpair(x, y);
}

intpair mapgen::getRandomWalkable(gridmapPtr m)
{
	auto pt = getRandomPoint(m);
	while (!m->isWalkable(pt))
		pt = getRandomPoint(m);
	return pt;
}


intpair mapgen::getRandomWithPad(gridmapPtr m, int pad)
{
	int x = randint(pad, m->_xsize - pad);
	int y = randint(pad, m->_ysize - pad);
	return intpair(x, y);
}

//	Random free point that's at least 'pad' points from any map edge
intpair mapgen::getFreeWithPad(gridmapPtr m, int pad)
{
	auto pt = getRandomWithPad(m, pad);
	while (!m->isPointClear(pt))
		pt = getRandomWithPad(m, pad);
	return pt;
}

intpair mapgen::getWalkableWithPad(gridmapPtr m, int pad)
{
	auto pt = getRandomWithPad(m, pad);
	while (!m->isWalkable(pt))
		pt = getRandomWithPad(m, pad);
	return pt;
}

intpair mapgen::getRandomInBounds(gridmapPtr m, int right_pad, int bottom_pad)
{
	int x = randrange(m->_xsize - right_pad);
	int y = randrange(m->_ysize - bottom_pad);
	return intpair(x, y);
}

intpair mapgen::getRandomInBounds(gridmapPtr m, int x, int y, int w, int h)
{
	int atx = randint(x, x + w);
	int aty = randint(y, y + h);
	return intpair(atx, aty);
}

intpair mapgen::getRandomFree(gridmapPtr m)
{
	auto pt = getRandomWalkable(m);
	while (m->getCreature(pt) != nullptr)
		pt = getRandomWalkable(m);
	return pt;
}

//	Random walkable square with no item on it.
intpair mapgen::getRandomForItem(gridmapPtr m)
{
	intpair pt;
	do {
		pt = getRandomWalkable(m);
	} while (m->getItem(pt) != nullptr);
	return pt;
}


//	Random square on which stairs can validly appear.
intpair mapgen::getRandomForStairs(gridmapPtr m)
{
	intpair pt;
	do {
		pt = getRandomWalkable(m);
	} while (m->getStairs(pt) != nullptr || !m->isTransparent(pt));
	return pt;
}



//	Random point in a circular region.
intpair mapgen::getRandomInCircle(gridmapPtr m, intpair ctr, int r)
{
	//	i guess we could do some silly stuff with rays and angles and whatnot
	//	but instead we just pick points in a square and reject points outside the circular region
	//	sorry. i truly apologize for doing this
	auto pt = getRandomInBounds(m, ctr.first - r, ctr.second - r, r * 2, r * 2);
	while (hypot(pt.first - ctr.first, pt.second - ctr.second) > r)
		pt = getRandomInBounds(m, ctr.first - r, ctr.second - r, r * 2, r * 2);
	return pt;
}


intpair mapgen::getRandomInNode(TCODBsp * node)
{
	int x = node->x + randint(0, node->w);
	int y = node->y + randint(0, node->h);
	return intpair(x, y);
}

intpair mapgen::getRandomWalkableInNode(gridmapPtr m, TCODBsp * node)
{
	auto pt = getRandomInNode(node);
	while (!m->isWalkable(pt))
		pt = getRandomInNode(node);
	return pt;
}


//	Returns a random point on the edge of the node, excluding the corners.
intpair mapgen::getRandomOnNodeEdge(TCODBsp * node)
{
	return getRandomPointOnEdge(node->x - 1, node->y - 1, node->w + 2, node->h + 2);
}


//	Returns a random point on an exterior edge of the given rectangle.
//	Will NOT return any of the corner points.
intpair mapgen::getRandomPointOnEdge(int x, int y, int w, int h)
{
	int fx, fy;
	if (coinflip())
	{
		fx = randint(x + 1, x + w - 2);
		if (coinflip())
			fy = y;
		else
			fy = y + h - 1;
	}
	else
	{
		fy = randint(y + 1, y + h - 2);
		if (coinflip())
			fx = x;
		else
			fx = x + w - 1;
	}
	return intpair(fx, fy);
}


//	Returns a random clear point on the map edge.
intpair mapgen::getRandomFreeOnEdge(gridmapPtr m)
{
	auto pt = getRandomPointOnEdge(0, 0, m->_xsize, m->_ysize);
	while (!m->isPointClear(pt))
		pt = getRandomPointOnEdge(0, 0, m->_xsize, m->_ysize);
	return pt;
}


//	Returns all walkable points within the given radius of the given central point.
vector<intpair> mapgen::getGroupOfFreePoints(gridmapPtr m, const intpair ctr, const int rad)
{
	vector<intpair> pts;
	for (int x = ctr.first - rad; x <= ctr.first + rad; x++)
	{
		for (int y = ctr.second - rad; y <= ctr.second + rad; y++)
		{
			if (m->inBounds(x, y) && m->isPointClear(x, y))
				pts.push_back(intpair(x, y));
		}
	}
	return pts;
}


//	Returns the number of neighbours of the given cell that are set to 'true'
const int mapgen::countNeighbours(const vector<vector<bool>>* cmap, const int cx, const int cy)
{
	int count = 0;
	for (int x = cx - 1; x < cx + 1; x++)
	{
		for (int y = cy - 1; y < cy + 1; y++)
		{
			if (!(x == cx && cy == cy) && (x >= 0 && x < cmap->size() && y >= 0 && y < cmap->at(x).size()))
			{
				if ((*cmap)[x][y])
					count++;
			}
		}
	}
	return count;
}

//	Cells 'die' if they have fewer than minPop or more than maxPop neighbours.
//	A cell is 'birthed' in an empty space if it has exactly 'birthPop' neighbours.
//	'startFilled' is the chance/100 that each cell starts populated.
vector<vector<bool>> mapgen::rollCellAutomata(const int x_sz, const int y_sz, const int minPop, const int maxPop, const int birthPop, int cycles, const int startFilled)
{
	//	Create the initial cell map.
	vector<vector<bool>> cellmap;
	vector<vector<bool>> swapmap;
	for (unsigned x = 0; x < x_sz; x++)
	{
		vector<bool> xcells;
		for (unsigned y = 0; y < y_sz; y++)
			xcells.push_back(roll_percent(startFilled));
		cellmap.push_back(xcells);
		swapmap.push_back(xcells);
	}

	//	Perform some cycles
	vector<vector<bool>> *current = &cellmap;
	vector<vector<bool>> *other = &swapmap;
	while (cycles-- > 0)
	{
		//	Perform one cycle of the current map.
		for (unsigned x = 0; x < x_sz; x++)
		{
			for (unsigned y = 0; y < y_sz; y++)
			{
				auto n = countNeighbours(current, x, y);
				(*other)[x][y] = (*current)[x][y];

				//	cell is alive - check for death
				if ((*current)[x][y])
				{
					if (n < minPop || n > maxPop)
						(*other)[x][y] = false;
				}

				//	cell is not alive - check for birth
				else
				{
					if (n == birthPop)
						(*other)[x][y] = true;
				}
			}
		}

		//	Switch the current map pointer to the updated map.
		if (current == &cellmap)
		{
			current = &swapmap;
			other = &cellmap;
		}
		else
		{
			current = &swapmap;
			other = &cellmap;
		}
	}

	return *current;
}




inline void mapgen::fillRegion(gridmapPtr m, vector<maptile> tileOptions, int x, int y, int w, int h)
{
	for (int atx = x; atx < x + w; atx++)
	{
		for (int aty = y; aty < y + h; aty++)
		{
			if (!m->inBounds(atx, aty))
				cout << "ERROR: fillRegion out of bounds at " << atx << ',' << aty << "; parameters " << x << ',' << y << ", w/h=" << w << ',' << h << endl;
			else
			{
				auto t = tileOptions[randrange(tileOptions.size())];
				m->setTile(t, atx, aty);
			}
		}
	}
}

inline void mapgen::fillRegion(gridmapPtr m, maptile t, int x, int y, int w, int h)
{
	fillRegion(m, vector<maptile>({ t }), x, y, w, h);
}

inline void mapgen::fillMap(gridmapPtr m, vector<maptile> tileOptions)
{
	fillRegion(m, tileOptions, 0, 0, m->_xsize, m->_ysize);
}

inline void mapgen::fillRegionCircular(gridmapPtr m, vector<maptile> tileOptions, int ctrx, int ctry, int rad)
{
	for (int atx = ctrx - rad; atx <= ctrx + rad; atx++)
	{
		for (int aty = ctry - rad; aty <= ctry + rad; aty++)
		{
			if (hypot(atx - ctrx, aty - ctry) <= rad && m->inBounds(atx, aty))
			{
				auto tl = tileOptions[randrange(tileOptions.size())];
				m->setTile(tl, atx, aty);
			}
		}
	}
}

inline void mapgen::fillRegionCircular(gridmapPtr m, maptile tl, int ctrx, int ctry, int rad)
{
	fillRegionCircular(m, vector<maptile>({ tl }), ctrx, ctry, rad);
}

//	Creates a 'room' type thing
inline void mapgen::fillRegionAndHollow(gridmapPtr m, maptile outer, vector<maptile> inner, int x, int y, int w, int h)
{
	fillRegion(m, outer, x, y, w, h);
	fillRegion(m, inner, x + 1, y + 1, w - 2, h - 2);
}

void mapgen::fillRegionWithSurface(gridmapPtr m, surface sf, int x, int y, int w, int h)
{
	for (unsigned atx = x; atx < x + w; atx++)
	{
		for (unsigned aty = y; aty < y + h; aty++)
		{
			if (canCoverWithSurface(m->getTile(atx, aty), sf))
				m->setSurface(sf, atx, aty);
		}
	}
}

void mapgen::fillRegionWithSurfaceCircular(gridmapPtr m, surface sf, intpair ctr, int rad)
{
	for (int atx = ctr.first - rad; atx <= ctr.first + rad; atx++)
	{
		for (int aty = ctr.second - rad; aty <= ctr.second + rad; aty++)
		{
			if (m->inBounds(atx, aty) && hypot(atx - ctr.first, aty - ctr.second) <= rad && m->inBounds(atx, aty))
			{
				if (canCoverWithSurface(m->getTile(atx, aty), sf))
					m->setSurface(sf, atx, aty);
			}
		}
	}
}



//	Scatters the given surface around in a given region.
void mapgen::scatterSurface(gridmapPtr m, surface sf, int x, int y, int w, int h, float density)
{
	int amt = (float)(w * h) * density;
	while (amt-- >= 0)
	{
		auto pt = getRandomInBounds(m, x, y, w, h);
		auto tl = m->getTile(pt);
		if (canCoverWithSurface(tl, sf))
			m->setSurface(sf, pt);
	}
}


//	Same as 'scatterTile', but won't replace non-walkable tiles like walls
void mapgen::scatterTileOnWalkable(gridmapPtr m, maptile tl, int x, int y, int w, int h, float density)
{
	int amt = (float)(w * h) * density;
	while (amt-- >= 0)
	{
		auto pt = getRandomInBounds(m, x, y, w, h);
		if (m->isWalkable(pt))
			m->setTile(tl, pt);
	}
}


//	Scatters the given tile around in a given region.
void mapgen::scatterTile(gridmapPtr m, maptile tl, int x, int y, int w, int h, float density)
{
	int amt = (float)(w * h) * density;
	while (amt-- >= 0)
	{
		auto pt = getRandomInBounds(m, x, y, w, h);
		m->setTile(tl, pt);
	}
}


//	Can appear anywhere on the map. Can over-write existing tiles.
void mapgen::scatterOnMap(gridmapPtr m, maptile tl, float density)
{
	scatterTile(m, tl, 0, 0, m->_xsize - 1, m->_ysize - 1, density);
}


//	Assembles a list of all leaf nodes in the tree by recursively iterating over all the nodes.
void mapgen::getLeafNodesRecursive(vector<TCODBsp*>* nodes, TCODBsp * thisNode)
{
	if (thisNode->isLeaf())
		nodes->push_back(thisNode);
	else
	{
		getLeafNodesRecursive(nodes, thisNode->getLeft());
		getLeafNodesRecursive(nodes, thisNode->getRight());
	}
}



//	Creates a BSP tree; returns a list of all leaf nodes.
vector<TCODBsp*> mapgen::createNodeMap(gridmapPtr m, const int min_size, const int recursions)
{
	//	Create the tree.
	auto root = new TCODBsp(2, 2, m->_xsize - 4, m->_ysize - 4);
	root->splitRecursive(NULL, recursions, min_size, min_size, 1.5f, 1.5f);

	//	Get every leaf node.
	vector<TCODBsp*> leaves;
	getLeafNodesRecursive(&leaves, root);

	return leaves;
}


//	Dig a tunnel that connects the two given points
void mapgen::tunnelBetweenPoints(gridmapPtr m, int midx1, int midy1, int midx2, int midy2, maptile floor)
{
	//	find tunneling directions
	int xv = 0, yv = 0;

	if (midx1 < midx2)		xv = 1;
	else if (midx1 > midx2)		xv = -1;
	if (midy1 < midy2)		yv = 1;
	else if (midy1 > midy2)		yv = -1;

	//	walk the path
	int atx = midx1, aty = midy1;
	bool vert_walk = randint(1, 2) == 1;
	while (atx != midx2 || aty != midy2)
	{
		//	decide whether to switch directions
		if (vert_walk && aty == midy2)
			vert_walk = false;
		else if (!vert_walk && atx == midx2)
			vert_walk = true;

		//	walk
		if (vert_walk && aty != midy2)
			aty += yv;
		else if (atx != midx2)
			atx += xv;
		//vert_walk = !vert_walk;

		//	set tile beneath
		m->setTile(floor, atx, aty);
	}
}


//	Digs a passage from one node to another.
void mapgen::tunnelBetweenNodes(gridmapPtr m, TCODBsp * node1, TCODBsp * node2, maptile floor)
{
	//	find the MIDPOINTS of each room to determine where they are relative to one another
	int midx1 = node1->x + node1->w / 2 + randint(-2, 2);
	int midy1 = node1->y + node1->h / 2 + randint(-2, 2);
	int midx2 = node2->x + node2->w / 2 + randint(-2, 2);
	int midy2 = node2->y + node2->h / 2 + randint(-2, 2);
	tunnelBetweenPoints(m, midx1, midy1, midx2, midy2, floor);
}


//	Generate a building at the given coordinates with the given dimensions.
//	A 'building' is just a square with a random hole for a door.
void mapgen::addBuilding(gridmapPtr m, int x, int y, int w, int h, maptile wall, maptile floor, maptile door)
{
	//	the 'building'
	fillRegionAndHollow(m, wall, { floor }, x, y, w, h);

	//	pick a random exterior point for the door
	auto door_pt = getRandomPointOnEdge(x, y, w, h);
	m->setTile(door, door_pt);
}


//	Adds a circular region, whose outer tiles all consist of the maptile 'outer'
//	and the rest of which is filled with 'inner'.
void mapgen::addLake(gridmapPtr m, intpair ctr, int r, maptile inner, maptile outer)
{
	for (int x = ctr.first - r; x <= ctr.first + r; x++)
	{
		for (int y = ctr.second - r; y <= ctr.second + r; y++)
		{
			if (m->inBounds(x, y))
			{
				int d = hypot(x - ctr.first, y - ctr.second);
				if (d == r)
					m->setTile(outer, x, y);
				else if (d < r)
					m->setTile(inner, x, y);
			}
		}
	}
}


//	Returns monster data object with the given ID.
monsterDataPtr mapgen::getMonsterDataWithId(const vector<monsterDataPtr>* mdata_table, const string id)
{
	for (auto mdat : *mdata_table)
	{
		if (mdat->_id == id)
			return mdat;
	}
	cout << "ERROR: Couldn't find a monster with id '" << id << "'" << endl;
	return nullptr;
}


//	Get a monster table with an ID matching the given zone id
monsterTablePtr mapgen::getTableForZone(loadedFileDataPtr ldata, string zone)
{
	for (auto mt : ldata->_monsterTables)
	{
		if (mt->_zone == zone)
			return mt;
	}
	return nullptr;
}


//	Returns a list of all tables that match the given zone id.
vector<monsterTablePtr> mapgen::getTablesForZone(loadedFileDataPtr ldata, string zone)
{
	vector<monsterTablePtr> mtables;
	for (auto mt : ldata->_monsterTables)
	{
		if (mt->_zone == zone)
			mtables.push_back(mt);
	}
	return mtables;
}


//	Adds a given qty of monsters from the given table to the map.
void mapgen::addMonstersFromTable(gridmapPtr m, loadedFileDataPtr ldata, monsterTablePtr mtable, int dl, int group_count)
{
	//	Get monsters with valid DLs on this table
	vector<monsterDataPtr> mlist;
	for (auto id : mtable->_ids)
	{
		auto mdat = getMonsterDataWithId(&ldata->_monsterData, id);
		if (mdat->_dangerLevel <= dl)
			mlist.push_back(mdat);
	}

	//	Did we find any monsters?!
	if (!mlist.empty())
	{
		//	Groups OR singles
		while (group_count-- > 0)
		{
			//	pick a group type
			auto mdat = mlist[randrange(mlist.size())];

			//	if this is a GROUP table, we add a pack of the monsters
			int mcount = 1;
			if (mtable->_mtype == MTABLE_PACK)
				mcount = randint(3, 6);

			//	emplace monsters
			auto pts = getGroupOfFreePoints(m, getRandomFree(m), 3);
			while (mcount-- > 0)
			{
				//	stop if we run out of points
				if (pts.empty())
					break;

				//	generate a monster & pick a place for it
				auto mon = monsterPtr(new monster(mdat->_tokens));
				auto p = randrange(pts.size());
				m->addCreature(mon, pts[p]);
				pts.erase(pts.begin() + p);
			}
		}
	}
}


//	Randomly scatter monsters around the map
void mapgen::addMonsters(gridmapPtr m, loadedFileDataPtr ldata, int dl, string zone)
{
	//	Get all monster table for this zone
	auto mtables = getTablesForZone(ldata, zone);

	//	Decide how to distribute groups from among these tables.
	int per_table = 10 / mtables.size();
	for (auto mt : mtables)
		addMonstersFromTable(m, ldata, mt, dl, per_table + randint(-1, 1));
}


//	Scatter some treasures around the map
void mapgen::addLoot(gridmapPtr m, int dl)
{
	auto items = lootgen::rollDropTable(dl);
	for (auto it : items)
	{
		auto pt = getRandomForItem(m);
		m->addItem(it, pt);
	}
}


//	Returns a string representation of non-walkable & walkable adjacent tiles (1=walkable,0=wall)
string mapgen::countAdjacentWalls(gridmapPtr m, int x, int y)
{
	string txt = "";
	for (int atx = x - 1; atx <= x + 1; atx++)
	{
		for (int aty = y - 1; aty <= y + 1; aty++)
		{
			//	what tile is here bois
			auto tl = m->getTile(atx, aty);
			if ((!m->isWalkable(atx, aty) && !m->isTransparent(atx, aty)) || (tl == TILE_DOOR_CELL || tl == TILE_DOOR_WOODEN))
				txt += "1";
			else
				txt += "0";
		}
	}
	return txt;
}

//	Decides whether a door should go here, based on the configuration of walls surrounding the point.
bool mapgen::verifyPointOkayForDoor(gridmapPtr m, int x, int y)
{
	auto wallstr = countAdjacentWalls(m, x, y);
	return wallstr == "101101000" || wallstr == "110000110" || wallstr == "000101101" || wallstr == "011000011";
}


//	Add doors of random types to any suitable points.
void mapgen::addDoorsToMap(gridmapPtr m)
{
	for (int x = 2; x < m->_xsize - 2; x++)
	{
		for (int y = 2; y < m->_ysize - 2; y++)
		{
			if (verifyPointOkayForDoor(m, x, y))
			{
				auto tl = TILE_DOOR_WOODEN;
				if (roll_one_in(4)) tl == TILE_DOOR_CELL;
				m->setTile(tl, x, y);
			}
		}
	}
}


//	Set of valid connections from the given map
vector<string> mapgen::getZoneOptions(string connectedTo, int for_dl)
{
	vector<string> options{ };
	if (for_dl <= 4)
		options.push_back(ZONE_ID_FORSAKEN_PRISON);
	if (for_dl > 1)
		options.push_back(ZONE_ID_SILENT_CITY);
	if (for_dl > 2)
		options.push_back(ZONE_ID_SUNKEN_RUINS);
	if (for_dl > 5)
		options.push_back(ZONE_ID_CAVES);
	if (for_dl > 8)
		options.push_back(ZONE_ID_SWAMP);
	return options;
}


//	Stairs to other levels, from the given level (zone id and danger level)
void mapgen::addStairs(gridmapPtr m, string zone, int dl)
{
	//	pick some connection options
	vector<string> options = getZoneOptions(zone, dl + 1);

	//	pick spots for them
	int scount = randint(1, 2);
	if (roll_one_in(4)) scount++;
	while (scount-- > 0)
	{
		auto pt = getRandomForStairs(m);
		auto zn = options[randrange(options.size())];
		m->addStairs(pt, zn, "stairs down", dl + 1, '>');
	}

	//	stairs going back up
	if (dl > 1)
	{
		scount = 1;
		while (roll_one_in(3)) scount++;
		options = getZoneOptions(zone, dl - 1);
		while (scount-- > 0)
		{
			auto pt = getRandomForStairs(m);
			auto zn = options[randrange(options.size())];
			m->addStairs(pt, zn, "stairs up", dl - 1, '<');
			m->_startPt = pt;
		}
	}

	//	at depth 1, we can go back up to fireholm
	else
	{
		auto pt = getRandomForStairs(m);
		m->addStairs(pt, ZONE_ID_HOME_BASE, "stairs to Fireholm", 0, '<');
	}
}


//	Puts down new stairs when we return to the home map
void mapgen::refreshHomeMapStairs(gridmapPtr m, int dl)
{
	auto options = getZoneOptions(ZONE_ID_HOME_BASE, dl);
	const vector<intpair> pts = { intpair(27, 44), intpair(23, 6), intpair(27, 6), intpair(23, 9), intpair(27, 9) };
	for (unsigned i = 0; i < options.size(); i++)
	{
		if (i >= pts.size())
			break;
		m->addStairs(pts[i], options[i], "stairs to " + getZoneName(options[i]), dl);
	}
}

//	room features
void mapgen::room_Prison(gridmapPtr m, TCODBsp* node)
{
	int r = randint(1, 9);
	if (r == 1)
	{
		scatterTile(m, TILE_MOSS, node->x, node->h, node->w - 1, node->h - 1, 0.05);
		scatterSurface(m, SURFACE_WATER, node->x, node->y, node->w - 1, node->h - 1, 0.75);
	}
	else if (r == 2 || node->w < 7 || node->h < 7)
	{
		scatterTile(m, TILE_WOODEN_TABLE, node->x + 2, node->y + 2, node->w - 4, node->h - 4, 0.1);
	}
	else
	{
		if (node->w >= 7 && node->h >= 7)
		{
			for (unsigned x = 1; x < node->w - 2; x += 5)
			{
				for (unsigned y = 1; y < node->h - 2; y += 5)
				{
					addBuilding(m, x + node->x, y + node->y, 3, 3, TILE_WALL, TILE_FLOOR, TILE_DOOR_CELL);
					intpair pt;
					do {
						pt = getRandomPointOnEdge(x + node->x, y + node->y, 3, 3);
					} while (m->getTile(pt) == TILE_DOOR_CELL);
					m->setTile(TILE_CELL_WINDOW, pt);
				}
			}
		}
	}
}



void mapgen::room_SilentRuins(gridmapPtr m, TCODBsp * node)
{
	int r = randint(1, 12);
	fillRegion(m, TILE_SAND, node->x, node->y, node->w, node->h);
	if (r == 1)
	{
		addLake(m, intpair(node->x + node->w / 2, node->y + node->h / 2), (node->w + node->h) / 3, TILE_WATER_DEEP, TILE_GRASS);
		scatterSurface(m, SURFACE_FROST, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.4);
		scatterSurface(m, SURFACE_ICE, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.4);
	}
	else if (r == 2)
	{
		scatterTile(m, TILE_ASH, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.4);
		scatterSurface(m, SURFACE_OIL, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.1);
		m->setTile(TILE_FIRE_EVERBURNING, node->x + node->w / 2, node->y + node->h / 2);
	}
	else if (r == 3)
	{
		scatterTile(m, TILE_CARPET_RED, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.4);
		scatterTile(m, TILE_STATUE_STONE, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.05);
	}
	else if (r < 7)
	{
		scatterTile(m, TILE_BONES, node->x + 1, node->y + 1, node->w - 3, node->h - 3, 0.4);
	}
}



void mapgen::room_SunkenCity(gridmapPtr m, TCODBsp * node)
{
	int r = randint(1, 10);
	if (r == 1)
	{
		fillRegion(m, { TILE_GRASS, TILE_GRASS, TILE_GRASS, TILE_MOSS, TILE_MOSS, TILE_BUSH }, node->x, node->y, node->w, node->h);
		scatterSurface(m, SURFACE_WATER, node->x, node->y, node->w, node->h, 0.5);
	}
	else if (r <= 3)
	{
		addLake(m, intpair(node->x + node->w / 2, node->y + node->h / 2), (node->w + node->h) / 3, TILE_WATER_DEEP, TILE_DIRT);
	}
	else if (r <= 5)
	{
		for (unsigned x = node->x + 1; x < node->x + node->w - 1; x += 2)
		{
			for (unsigned y = node->y + 1; y < node->y + node->h - 1; y += 2)
				m->setTile(TILE_WALL, x, y);
		}
	}
	else if (r <= 7)
	{
		fillRegionWithSurface(m, SURFACE_WATER, node->x, node->y, node->w, node->h);
	}
}



//	Randomly rolls one dimensional axis for a map. Larger maps are increasingly unlikely.
int mapgen::rollMapDimension()
{
	int sz = 40;
	while (roll_percent(130 - sz))
		sz += 5;
	return sz;
}


//	Cellular-automata based.
void mapgen::style_Cave(gridmapPtr m)
{
	//	basic geometry
	auto cmap = rollCellAutomata(m->_xsize, m->_ysize, 2, 4, 3, 5);
	for (unsigned x = 0; x < m->_xsize; x++)
	{
		for (unsigned y = 0; y < m->_ysize; y++)
		{
			if (!cmap[x][y])
				m->setTile(TILE_FLOOR, x, y);
			else
				m->setTile(TILE_WALL, x, y);
		}
	}

	//	random features
	if (roll_one_in(3))
		scatterTile(m, TILE_MOSS, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.1);
	if (roll_one_in(3))
		scatterTile(m, TILE_GRASS, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.1);
	if (roll_one_in(6))
	{
		scatterTile(m, TILE_ASH, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.4);
		scatterTile(m, TILE_BONES, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.4);
		scatterSurface(m, SURFACE_CORPSE, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.8);
	}
	if (roll_one_in(3))
		scatterSurface(m, SURFACE_WATER, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.1);
	if (roll_one_in(4))
		scatterSurface(m, SURFACE_FROST, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.8);
	while (roll_one_in(2))
	{
		int r = randint(3, 5);
		auto pt = getRandomWithPad(m, r + 1);
		fillRegionCircular(m, TILE_WATER_DEEP, pt.first, pt.second, r);
	}
	if (roll_one_in(6))
	{
		int r = randint(3, 12);
		fillRegionWithSurfaceCircular(m, SURFACE_OIL, getRandomWithPad(m, r + 1), r);
	}
	if (roll_one_in(6))
	{
		int r = randint(2, 5);
		auto pt = getRandomWithPad(m, r + 1);
		fillRegionCircular(m, TILE_WATER_PUTRID, pt.first, pt.second, r);
	}
}


//	Rooms of different sizes joined together by passages.
void mapgen::style_Rooms(gridmapPtr m, string zone, int dl)
{
	fillMap(m, { TILE_WALL });

	//	sometimes vary the min room size & the recursion depth to create a slightly different feel
	int min_size = 8, recurse = 10;
	int r = randint(1, 100);
	if (r <= 30)
	{
		min_size = 5;
		recurse = 15;
	}
	else if (r <= 35)
		min_size = 15;
	auto leaves = createNodeMap(m, min_size, recurse);


	//	make each node into a room
	for (auto leaf : leaves)
	{
		//	shape the room
		leaf->resize(leaf->x + 1, leaf->y + 1, leaf->w - 2, leaf->h - 2);
		fillRegion(m, TILE_FLOOR, leaf->x, leaf->y, leaf->w, leaf->h);
	}


	//	connect random nodes together
	vector<TCODBsp*> toConnect;
	for (auto l : leaves)
		toConnect.push_back(l);
	for (auto leaf : leaves)
	{
		//	if only one leaf remains to connect & it the same as the last leaf in the list, just quit
		//	averts an endless loop here
		if (toConnect.size() == 1 && leaf == toConnect[0])
			break;

		//	pick a DIFFERENT node to link to
		unsigned n;
		do {
			n = randrange(toConnect.size());
		} while (toConnect[n] == leaf);

		//	link to it
		tunnelBetweenNodes(m, leaf, toConnect[n], TILE_FLOOR);

		//	remove the linked node so we don't link to it again
		toConnect.erase(toConnect.begin() + n);
	}


	//	Add room features
	for (auto leaf : leaves)
	{
		if		(zone == ZONE_ID_FORSAKEN_PRISON)	room_Prison(m, leaf);
		else if (zone == ZONE_ID_SUNKEN_RUINS)		room_SunkenCity(m, leaf);
		else if (zone == ZONE_ID_SILENT_CITY)		room_SilentRuins(m, leaf);
	}


	//	Additional stuff not restricted to rooms
	if (zone == ZONE_ID_SUNKEN_RUINS)
	{
		scatterTileOnWalkable(m, TILE_MOSS, 1, 1, m->_xsize - 2, m->_ysize - 2, 0.03);
		scatterTileOnWalkable(m, TILE_GRASS, 1, 1, m->_xsize - 2, m->_ysize - 2, 0.05);
		scatterSurface(m, SURFACE_WATER, 1, 1, m->_xsize - 2, m->_ysize - 2, 0.05);
		addDoorsToMap(m);
	}
}


void mapgen::style_Swamp(gridmapPtr m, string zone, int dl)
{
	fillMap(m, { TILE_GRASS, TILE_GRASS, TILE_DIRT, TILE_DIRT, TILE_MOSS, TILE_ASH, TILE_BUSH, });
	
	auto lcount = randint(3, 5);
	while (lcount-- > 0)
	{
		int r = randint(6, 10);
		auto pt = getRandomWithPad(m, r + 1);
		addLake(m, pt, r, TILE_WATER_DEEP, TILE_GRASS);
	}

	lcount = randint(0, 3);
	while (lcount-- > 0)
	{
		int r = randint(2, 4);
		auto pt = getRandomWithPad(m, r + 1);
		addLake(m, pt, r, TILE_WATER_PUTRID, TILE_GRASS);
	}

	scatterOnMap(m, TILE_TREE, 0.03);

	auto leaves = createNodeMap(m);
	for (auto leaf : leaves)
	{
		if (roll_one_in(8))
		{
			addBuilding(m, leaf->x, leaf->y, leaf->w, leaf->h, TILE_WALL, TILE_FLOOR, TILE_DOOR_WOODEN);
			scatterTile(m, TILE_WOODEN_TABLE, leaf->x + 2, leaf->y + 2, leaf->w - 4, leaf->h - 4, 0.05);
			scatterTile(m, TILE_MOSS, leaf->x, leaf->y, leaf->w, leaf->h, 0.1);
		}
	}

	scatterSurface(m, SURFACE_WATER, 1, 1, m->_xsize - 3, m->_ysize - 3, 0.3);
}


//	testing a more generic type of mapgen
gridmapPtr mapgen::generate_type2(loadedFileDataPtr ldata, string zone, int dl)
{
	auto m = gridmapPtr(new gridmap(rollMapDimension(), rollMapDimension()));
	m->_name = getZoneName(zone);
	

	//	base map style
	if (zone == ZONE_ID_CAVES)
		style_Cave(m);
	else if (zone == ZONE_ID_SWAMP)
		style_Swamp(m, zone, dl);
	else
		style_Rooms(m, zone, dl);


	//	Map exterior is always filled with indestructible walls.
	for (unsigned x = 0; x < m->_xsize; x++)
	{
		m->setTile(TILE_WALL_SUPER, x, 0);
		m->setTile(TILE_WALL_SUPER, x, m->_ysize - 1);
	}
	for (unsigned y = 0; y < m->_ysize; y++)
	{
		m->setTile(TILE_WALL_SUPER, 0, y);
		m->setTile(TILE_WALL_SUPER, m->_xsize - 1, y);
	}

	//	links to other levels
	addStairs(m, zone, dl);

	//	monsters/items
	addMonsters(m, ldata, dl, zone);
	addLoot(m, dl);

	//	random starting position
	m->_startPt = getRandomFree(m);
	m->updateTmap();

	return m;
}



//	permanent hub map
gridmapPtr mapgen::generate_HomeBase()
{
	auto m = gridmapPtr(new gridmap(51, 51));
	fillMap(m, { TILE_WALL_SUPER });

	//	central room
	fillRegion(m, TILE_FLOOR, 9, 8, 33, 32);
	scatterTile(m, TILE_ASH, 9, 8, 32, 31, 0.05);

	//	hall of exits
	fillRegion(m, TILE_WALL_SUPER, 21, 7, 9, 5);
	fillRegion(m, TILE_FLOOR, 22, 7, 7, 4);
	fillRegion(m, TILE_FLOOR, 9, 1, 33, 6);
	fillRegion(m, TILE_CARPET_RED, 10, 2, 31, 3);
	m->setTile(TILE_STATUE_STONE, 11, 3);
	m->setTile(TILE_STATUE_STONE, 39, 3);

	//	big carpets
	fillRegion(m, TILE_CARPET_RED, 4, 23, 38, 3);
	fillRegion(m, TILE_CARPET_RED, 24, 2, 3, 39);

	//	central fire
	fillRegion(m, TILE_FIRE_EVERBURNING, 24, 23, 3, 3);
	m->setTile(TILE_WALL, 23, 22);
	m->setTile(TILE_WALL, 23, 26);
	m->setTile(TILE_WALL, 27, 22);
	m->setTile(TILE_WALL, 27, 26);

	//	entrance to the map
	fillRegion(m, TILE_CARPET_RED, 22, 41, 7, 7);
	m->setTile(TILE_STATUE_STONE, 23, 42);
	m->setTile(TILE_STATUE_STONE, 23, 46);
	m->setTile(TILE_STATUE_STONE, 27, 42);
	m->setTile(TILE_STATUE_STONE, 27, 46);
	m->setTile(TILE_STASH_EQUIPMENT, 23, 44);
	m->_startPt = intpair(25, 44);

	//	garden
	m->setTile(TILE_DOOR_WOODEN, 42, 24);
	fillRegion(m, TILE_GRASS, 43, 8, 7, 40);
	fillRegion(m, TILE_GRASS, 30, 41, 20, 7);
	scatterTile(m, TILE_BUSH, 43, 8, 6, 39, 0.05);
	scatterTile(m, TILE_BUSH, 30, 41, 19, 6, 0.05);
	fillRegionCircular(m, TILE_WATER_DEEP, 46, 11, 2);
	fillRegionCircular(m, TILE_WATER_DEEP, 33, 44, 2);


	//	exit
	m->addStairs(intpair(27, 44), ZONE_ID_FORSAKEN_PRISON, "stairs down", 1, '>');


	//	shops
	m->addShop(lootgen::generate_AmmoStore(), intpair(29, 36));
	m->addShop(lootgen::generate_AlchemyShop(), intpair(17, 36));
	m->addShop(lootgen::generate_PotionShop(), intpair(18, 38));
	m->addShop(lootgen::generate_SpellShop(), intpair(28, 37));


	//	finish
	m->_name = "Fireholm";
	m->updateTmap();
	return m;
}


//	Creates a map using the mapgen template with the given id.
gridmapPtr mapgen::generate(loadedFileDataPtr ldata, string zone, int dl, bool allow_zone_change)
{
	//	Generate an empty map; extract map data
	auto m = gridmapPtr(new gridmap(rollMapDimension(), rollMapDimension()));
	m->_name = getZoneName(zone);
	m->_maptype = zone;


	//	Map exterior is always filled with indestructible walls.
	for (unsigned x = 0; x < m->_xsize; x++)
	{
		m->setTile(TILE_WALL_SUPER, x, 0);
		m->setTile(TILE_WALL_SUPER, x, m->_ysize - 1);
	}
	for (unsigned y = 0; y < m->_ysize; y++)
	{
		m->setTile(TILE_WALL_SUPER, 0, y);
		m->setTile(TILE_WALL_SUPER, m->_xsize - 1, y);
	}


	//	other stuff
	addStairs(m, zone, dl);
	addLoot(m, dl);
	addMonsters(m, ldata, dl, zone);


	//	Finalization
	m->_startPt = getRandomFree(m);
	m->updateTmap();
	return m;
}


string getZoneName(const string zone_id)
{
	if (zone_id == ZONE_ID_CAVES)
		return "Desolate Caves";
	else if (zone_id == ZONE_ID_FORSAKEN_PRISON)
		return "Forsaken Prison";
	else if (zone_id == ZONE_ID_SILENT_CITY)
		return "Silent City";
	else if (zone_id == ZONE_ID_SUNKEN_RUINS)
		return "Sunken Ruins";
	else if (zone_id == ZONE_ID_SWAMP)
		return "Deep Swamp";
	else if (zone_id == ZONE_ID_TOMBLANDS)
		return "Tomb Lands";
	else
		return "zone " + zone_id;
}
