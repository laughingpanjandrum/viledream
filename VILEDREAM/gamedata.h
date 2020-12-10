#pragma once

#include "animation.h"
#include "dataLoader.h"
#include "gridmap.h"
#include "player.h"


#define MESSAGE_COUNT_MAX 14


enum GameState
{
	STATE_NORMAL,
	STATE_LOOK, STATE_EXAMINE_MONSTER,
	STATE_CHARACTER_SHEET, STATE_RAISE_ATTRIBUTE, STATE_VIEW_FEATS, STATE_LEARN_FEAT,
	STATE_PICKUP_ITEM,
	STATE_VIEW_SHOP,
	STATE_AIM_RANGED, STATE_AIM_SPELL, STATE_AIM_THROWN,
	STATE_VIEW_INVENTORY, STATE_VIEW_SPELLS,
	STATE_VIEW_EQUIPMENT, STATE_MODIFY_EQUIPMENT,
	STATE_SELECT_ITEM_TO_EQUIP, STATE_SELECT_ITEM_TO_USE, STATE_SELECT_ITEM_TO_THROW,
	STATE_SELECT_SPELL_TO_CAST,
};

struct message
{
	message() {}
	message(string txt, colorType col) : _txt(txt), _color(col) {}
	string _txt;
	colorType _color;
};


struct gamedata
{
	//	Basic info
	gridmapPtr _map;
	playerPtr _player;

	//	State
	GameState _state;
	itemPtr _pickingUp;
	itemPtr _throwing;
	Spell _casting;
	int _idx;
	int _lastIdx;
	ItemCategory _invCategory;
	itemShopPtr _currentShop;
	
	//	Cursor/targeting
	intpair _cursorPt;
	vector<creaturePtr> _visibleMonsters;
	creaturePtr _target;

	//	Automoving
	int _automoveDist;
	intpair _automoveVec;

	//	Inventogry
	vector<itemPtr> _carriedAmmo;
	vector<itemPtr> _carriedBombs;
	vector<itemPtr> _carriedConsumables;
	vector<vector<itemPtr>> _carriedEquipment;
	int _gold = 0;

	//	Messages/animations
	vector<message> _messages;
	vector<animationPtr> _animations;

	//	Map progress
	gridmapPtr _homeBase;
	int _depth;

	//	Monsters waiting to spawn
	vector<string> _triggerMonsterSpawnId;
	vector<intpair> _triggerMonsterSpawnPt;
	vector<int> _triggerMonsterSpawnDamage;

	//	State changes waiting to be triggered
	bool _triggerRaiseAttribute = false;

	//	Debug
	bool _omniscient = false;
};
typedef shared_ptr<gamedata> gamedataPtr;


inline void addAnimation(gamedataPtr gdata, animationPtr anim)
{
	gdata->_animations.push_back(anim);
}

inline bool isCreatureInVisibleMonsterList(gamedataPtr gdata, creaturePtr cr)
{
	return find(gdata->_visibleMonsters.begin(), gdata->_visibleMonsters.end(), cr) != gdata->_visibleMonsters.end();
}