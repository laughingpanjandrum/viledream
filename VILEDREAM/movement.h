#pragma once

#include "gamedata.h"
#include "messages.h"
#include "combat.h"
#include "manageItems.h"



//	Tile effects.

void standOnTile(gamedataPtr gdata, creaturePtr cr);
void enterTile(gamedataPtr gdata, creaturePtr cr);


void knockback(gamedataPtr gdata, creaturePtr target, intpair awayFrom, int dist);
void pull(gamedataPtr gdata, creaturePtr target, intpair to, int dist);


//	Player movement

void playerBumpTile(gamedataPtr gdata, const intpair pt);

void setPlayerPosition(gamedataPtr gdata, intpair pt);
void playerEnterTile(gamedataPtr gdata, intpair pt);
bool doPlayerMove(gamedataPtr gdata, const intpair vec, bool allowAttacking = true);

void startAutoMoving(gamedataPtr gdata, const intpair vec);
void doAutomove(gamedataPtr gdata);