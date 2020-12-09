#pragma once

#include <REXSpeeder.h>
#include "inputHandler.h"
#include "color_constants.h"
#include "window.h"
#include "gamedata.h"
#include "manageItems.h"
#include "progression.h"
#include "visibility.h"

typedef shared_ptr<xp::RexImage> rexImagePtr;


#define MSG_COLOR_CHANGE_MARK '#'
#define MSG_DEFAULT_COLOR_MARK '@'


#define MAP_X_SIZE 51
#define MAP_Y_SIZE 41
#define MAP_X_OFFSET 3
#define MAP_Y_OFFSET 3


class display
{
public:

	display();
	~display() {}


	//	Character info

	void drawAttackByline(int x, int y, creaturePtr cr, itemPtr wp, bool is_ranged = false);
	void drawCharacterSheet(gamedataPtr gdata);
	void drawSpellList(gamedataPtr gdata);
	void drawEquipment(gamedataPtr gdata);
	
	void drawAttributeSelection(gamedataPtr gdata);
	void drawFeatsKnown(gamedataPtr gdata);
	void drawFeatsToBuy(gamedataPtr gdata);

	void drawMonsterInfo(gamedataPtr gdata, monsterPtr mon);


	//	Items

	void drawInventoryHeader(ItemCategory cat);
	void drawInventory(gamedataPtr gdata, ItemCategory cat, bool show_header = true);
	void drawEquipmentPickup(gamedataPtr gdata, itemPtr it);

	void drawItemInfo(gamedataPtr gdata, itemPtr it, int atx, int aty, bool showDescription = true);
	void drawWeaponInfo(gamedataPtr gdata, itemPtr it, int atx, int aty);
	void drawArmourInfo(gamedataPtr gdata, itemPtr it, int atx, int aty);
	void drawCloakInfo(gamedataPtr gdata, itemPtr it, int atx, int aty);
	void drawShieldInfo(gamedataPtr gdata, itemPtr it, int atx, int aty);


	//	REXPaint images

	void loadImages();
	void drawRXPImage(rexImagePtr img, int x, int y);


	//	The main interface

	void updateVisibleMapData(gamedataPtr gdata);
	void drawMainInterface(gamedataPtr gdata);

	void drawSidebar(gamedataPtr gdata);
	void drawMessages(gamedataPtr gdata);
	void drawMouseover(gamedataPtr gdata);

	void drawCreatureInfo(gamedataPtr gdata, creaturePtr t, int atx, int aty);


	//	Coordinate conversion

	bool isPointOnDisplay(const intpair pt);
	intpair mapToDisplayCoord(const intpair ctr, int x, int y);
	intpair displayToMapCoord(const intpair ctr, int x, int y);


	//	Special colours

	colorType getPercentColor(int per) const;
	colorType getResistantValueColor(int val) const;


	//	Animations

	void tickAnimations(gamedataPtr gdata);


	//	Utility

	void writeFormatted(int atx, int aty, string txt, vector<colorType> colors, float darkenFactor = 0.0f);

	void drawDottedLinePair(int x, int y, int x2, const string title, const string val, colorType val_col = COLOR_LIGHT, colorType title_col = COLOR_MEDIUM);
	void drawProgressDots(int x, int y, int filled, int maxval, colorType col);

	void drawLineHorizontal(int x, int y, int width, colorType color);
	void drawLineVertical(int x, int y, int height, colorType color);

	void clear() { _win.clear(); }
	void refresh() { _win.refresh(); }
	void waitForKeypress() { _win.waitForKeypress(); }

	string getstr(int x, int y, string prompt) { return _win.getstr(x, y, prompt); }


private:

	window _win;

	rexImagePtr _img_Main;

	//	Baked visible map data
	vector<vector<int>> _visibleGlyphs;
	vector<vector<colorType>> _visibleColors;
	vector<vector<colorType>> _visibleBgcolors;

	TCODColor _percentColors[101];

};

