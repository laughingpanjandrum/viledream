#pragma once

#include "gamedata.h"
#include "monster.h"
#include "visibility.h"
#include "combat.h"
#include "movement.h"


#define AI_MAX_DETECTION_RADIUS 14



namespace ai
{


	//		Movement.

	void executeMove(gamedataPtr gdata, monsterPtr ai, intpair to);

	bool moveToPoint(gamedataPtr data, monsterPtr ai, intpair toPt);
	bool moveAwayFromPoint(gamedataPtr gdata, monsterPtr ai, intpair awayFrom);



	//		Combat.

	void doCombatAction(gamedataPtr gdata, monsterPtr ai);




	//		Target finding.

	bool canTargetCreature(gamedataPtr gdata, monsterPtr ai, creaturePtr t);

	void checkTargetValidity(gamedataPtr gdata, monsterPtr ai);

	void setTarget(gamedataPtr gdata, monsterPtr ai, creaturePtr t);
	void findMonsterTarget(gamedataPtr gdata, monsterPtr ai);
	void findTarget(gamedataPtr gdata, monsterPtr ai);



	//		Special abilities.

	bool tryUseSpecialAbility(gamedataPtr gdata, monsterPtr ai);
	bool tryTouchAttack(gamedataPtr gdata, monsterPtr ai, creaturePtr t);



	//	Turns all start here.
	void takeTurn(gamedataPtr gdata, monsterPtr ai);

}