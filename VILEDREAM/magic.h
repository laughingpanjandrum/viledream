#pragma once

#include "combat.h"
#include "gamedata.h"
#include "movement.h"



//	Spell knowledge

void playerGetSpellbook(gamedataPtr gdata, itemPtr it);


//	Selecting/viewing spells

void openSpellList(gamedataPtr gdata);

void openSpellSelectMenu(gamedataPtr gdata);
void selectSpellToCast(gamedataPtr gdata, const int idx);


//	Casting spells

bool rollSpellAttack(gamedataPtr gdata, creaturePtr target);

void expendSpellCost(gamedataPtr gdata, const Spell sp);
void triggerInstantSpell(gamedataPtr gdata, const Spell sp);
void triggerSpellInDirection(gamedataPtr gdata, const Spell sp, const intpair vec);


//	Spell effects

void spell_AllVisible(gamedataPtr gdata, const Spell sp);
void spell_Burst(gamedataPtr gdata, const Spell sp, const int rad);
void spell_Penetrating(gamedataPtr gdata, const Spell sp, const intpair vec);
void spell_RandomTarget(gamedataPtr gdata, const Spell sp);
void spell_Ray(gamedataPtr gdata, const Spell sp, const intpair vec);

void spell_Electrify(gamedataPtr gdata);