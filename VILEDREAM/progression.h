#pragma once

#include "gamedata.h"
#include "messages.h"




//	Feats

void openFeatsKnown(gamedataPtr gdata);
void learnFeatForSlot(gamedataPtr gdata, const int idx);
void equipNewFeatInSlot(gamedataPtr gdata, const int idx, const int inSlot);


//	Leveling up skills

void playerRaiseAttribute(gamedataPtr gdata, const int idx);



//	XP and leveling

int getXPAsPercent(gamedataPtr gdata);

int getXPForLevel(int lvl);
void playerAddLevel(gamedataPtr gdata);

void addKillXP(gamedataPtr gdata, int kill_dl);