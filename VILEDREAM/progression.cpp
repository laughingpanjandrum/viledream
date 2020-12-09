#include "progression.h"


//	Player's current xp as a percent of the amt required to level up.
int getXPAsPercent(gamedataPtr gdata)
{
	return 100.0f * (float)gdata->_player->_xp / (float)getXPForLevel(gdata->_player->_level + 1);
}


//	Shows feats we have memorized.
void openFeatsKnown(gamedataPtr gdata)
{
	gdata->_state = STATE_VIEW_FEATS;
	gdata->_idx = -1;
}


//	Select a new feat to learn for a particular slot.
void learnFeatForSlot(gamedataPtr gdata, const int idx)
{
	if (idx >= 0 && idx < gdata->_player->_featSlots.size())
	{
		//	feat slot must be empty and available
		if (gdata->_player->isFeatSlotAvailable(idx))
		{
			gdata->_lastIdx = idx;
			gdata->_idx = -1;
			gdata->_state = STATE_LEARN_FEAT;
		}
		else
		{
			gdata->_idx = -1;
			messages::add(gdata, "Level up to unlock this slot!", COLOR_NEGATIVE);
		}
	}
}


//	If we don't already know the feat at 'idx' in ALL_FEATS, place it in the player feat slot 'inSlot'
void equipNewFeatInSlot(gamedataPtr gdata, const int idx, const int inSlot)
{
	if (idx >= 0 && idx < ALL_FEATS.size())
	{
		auto ft = ALL_FEATS[idx];
		if (!gdata->_player->knowsFeat(ft))
		{
			gdata->_player->_featSlots[inSlot] = ft;
			openFeatsKnown(gdata);
		}
	}
}


//	Increase the selected attribute, if it's valid
void playerRaiseAttribute(gamedataPtr gdata, const int idx)
{
	if (idx >= 0 && idx < ATTR__MAX)
	{
		auto attr = static_cast<Attribute>(idx);
		gdata->_player->increaseAttribute(attr);
		string msg = "You feel ";
		switch (attr)
		{
		case(ATTR_AGILITY):			msg += "more agile!"; break;
		case(ATTR_CHARISMA):		msg += "more charismatic!"; break;
		case(ATTR_CONSTITUTION):	msg += "more durable!"; break;
		case(ATTR_DEXTERITY):		msg += "more dexterous!"; break;
		case(ATTR_STRENGTH):		msg += "stronger!"; break;
		}
		messages::add(gdata, msg, COLOR_WHITE);
		gdata->_state = STATE_NORMAL;
	}
}


//	How much XP is required to attain the given level
int getXPForLevel(int lvl)
{
	if		(lvl <= 5)	return lvl * 50;
	else if (lvl <= 10)	return lvl * 200;
	else if (lvl <= 20)	return lvl * 500;
	else if (lvl <= 30)	return lvl * 1000;
	else				return lvl * 2500;
}

void playerAddLevel(gamedataPtr gdata)
{
	gdata->_player->_level++;
	gdata->_player->_xp -= getXPForLevel(gdata->_player->_level);
	messages::add(gdata, " ***", COLOR_WHITE);
	messages::add(gdata, "  Welcome to level " + to_string(gdata->_player->_level) + "!", COLOR_WHITE);
	messages::add(gdata, " ***", COLOR_WHITE);


	//	health boost: fixed initially, randomized later
	if (gdata->_player->_level <= 5)
		gdata->_player->increaseBaseHealth(5);
	else
		gdata->_player->increaseBaseHealth(randint(2, 5));


	//	stat increase on even-numbered levels
	if (gdata->_player->_level % 2 == 0)
	{
		gdata->_triggerRaiseAttribute = true;
	}
}

void addKillXP(gamedataPtr gdata, int kill_dl)
{
	int xp = kill_dl * randint(10, 20);
	gdata->_player->_xp += xp;
	if (gdata->_player->_xp >= getXPForLevel(gdata->_player->_level + 1))
		playerAddLevel(gdata);
}
