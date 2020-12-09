#pragma once

#include "libtcod.hpp"
#include "utility.h"
#include "gridmap.h"
#include "dataLoader.h"
#include "monster.h"
#include "lootgen.h"


#define ZONE_ID_HOME_BASE			"fireholm"

#define ZONE_ID_FORSAKEN_PRISON		"forsaken_prison"

#define ZONE_ID_CAVES				"caves"
#define ZONE_ID_SILENT_CITY			"silent_city"
#define ZONE_ID_SUNKEN_RUINS		"sunken_ruins"
#define ZONE_ID_SWAMP				"swamp"
#define ZONE_ID_TOMBLANDS			"tomblands"


string getZoneName(const string zone_id);



namespace mapgen
{


	//		Getting random points.

	intpair getRandomPoint(gridmapPtr m);
	intpair getRandomWalkable(gridmapPtr m);
	intpair getRandomInBounds(gridmapPtr m, int right_pad, int bottom_pad);
	intpair getRandomInBounds(gridmapPtr m, int x, int y, int w, int h);
	intpair getRandomFree(gridmapPtr m);
	intpair getRandomForItem(gridmapPtr m);
	intpair getRandomForStairs(gridmapPtr m);
	intpair getRandomInCircle(gridmapPtr m, intpair ctr, int r);

	intpair getRandomWithPad(gridmapPtr m, int pad);
	intpair getFreeWithPad(gridmapPtr m, int pad);
	intpair getWalkableWithPad(gridmapPtr m, int pad);

	intpair getRandomInNode(TCODBsp* node);
	intpair getRandomWalkableInNode(gridmapPtr m, TCODBsp* node);

	intpair getRandomOnNodeEdge(TCODBsp* node);
	intpair getRandomPointOnEdge(int x, int y, int w, int h);
	intpair getRandomFreeOnEdge(gridmapPtr m);

	vector<intpair> getGroupOfFreePoints(gridmapPtr m, const intpair ctr, const int rad);


	//		Cellular automata

	const int countNeighbours(const vector<vector<bool>>* cmap, const int cx, const int cy);
	vector<vector<bool>> rollCellAutomata(const int x_sz, const int y_sz, const int minPop, const int maxPop, const int birth, int cycles, const int startFilled = 50);


	//		Fillers.

	void fillRegion(gridmapPtr m, vector<maptile> tileOptions, int x, int y, int w, int h);
	void fillRegion(gridmapPtr m, maptile t, int x, int y, int w, int h);
	void fillMap(gridmapPtr m, vector<maptile> tileOptions);

	void fillRegionCircular(gridmapPtr m, vector<maptile> tileOptions, int ctrx, int ctry, int rad);
	void fillRegionCircular(gridmapPtr m, maptile tl, int ctrx, int ctry, int rad);
	void fillRegionAndHollow(gridmapPtr m, maptile outer, vector<maptile> inner, int x, int y, int w, int h);

	void fillRegionWithSurface(gridmapPtr m, surface sf, int x, int y, int w, int h);
	void fillRegionWithSurfaceCircular(gridmapPtr m, surface sf, intpair ctr, int rad);


	//		Scatterers.

	void scatterSurface(gridmapPtr m, surface sf, int x, int y, int w, int h, float density);
	
	void scatterTileOnWalkable(gridmapPtr m, maptile tl, int x, int y, int w, int h, float density);
	void scatterTile(gridmapPtr m, maptile tl, int x, int y, int w, int h, float density);
	void scatterOnMap(gridmapPtr m, maptile tl, float density);


	//		BSP nodes.

	void getLeafNodesRecursive(vector<TCODBsp*>* nodes, TCODBsp* thisNode);
	vector<TCODBsp*> createNodeMap(gridmapPtr m, const int min_size = 10, const int recursions = 10);

	void tunnelBetweenPoints(gridmapPtr m, int midx1, int midy1, int midx2, int midy2, maptile floor);
	void tunnelBetweenNodes(gridmapPtr m, TCODBsp* node1, TCODBsp* node2, maptile floor);


	//		Special structures.

	void addBuilding(gridmapPtr m, int x, int y, int w, int h, maptile wall, maptile floor, maptile door);
	void addLake(gridmapPtr m, intpair ctr, int r, maptile inner, maptile outer);



	//		Monster generation

	monsterDataPtr getMonsterDataWithId(const vector<monsterDataPtr>* mdata_table, const string id);
	monsterTablePtr getTableForZone(loadedFileDataPtr ldata, string zone);
	vector<monsterTablePtr> getTablesForZone(loadedFileDataPtr ldata, string zone);

	void addMonstersFromTable(gridmapPtr m, loadedFileDataPtr ldata, monsterTablePtr mtable, int dl, int group_count);
	void addMonsters(gridmapPtr m, loadedFileDataPtr ldata, int dl, string zone);


	//		Loot

	void addLoot(gridmapPtr m, int dl);


	//		Doors

	string countAdjacentWalls(gridmapPtr m, int x, int y);
	bool verifyPointOkayForDoor(gridmapPtr m, int x, int y);
	void addDoorsToMap(gridmapPtr m);


	//		Connections to other maps


	vector<string> getZoneOptions(string connectedTo, int for_dl);
	void addStairs(gridmapPtr m, string zone, int dl);
	void refreshHomeMapStairs(gridmapPtr m, int dl);
	

	//		Features

	
	void room_Prison(gridmapPtr m, TCODBsp* node);
	void room_SilentRuins(gridmapPtr m, TCODBsp* node);
	void room_SunkenCity(gridmapPtr m, TCODBsp* node);



	//		Map generation

	int rollMapDimension();

	void style_Cave(gridmapPtr m);
	void style_Rooms(gridmapPtr m, string zone, int dl);
	void style_Swamp(gridmapPtr m, string zone, int dl);

	gridmapPtr generate_type2(loadedFileDataPtr ldata, string zone, int dl);

	gridmapPtr generate_HomeBase();
	gridmapPtr generate(loadedFileDataPtr ldata, string zone, int dl, bool allow_zone_change);

}