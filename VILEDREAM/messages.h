#pragma once

#include "gamedata.h"


namespace messages
{
	void add(gamedataPtr gdata, string txt, colorType col);

	
	//	Weapon attacks
	void combat_hit(gamedataPtr gdata, creaturePtr attacker, creaturePtr target, int dmg, bool crit = false);
	void combat_touch(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);
	void combat_miss(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);
	void combat_block(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);
	void combat_riposte(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);
	void combat_reprisal(gamedataPtr gdata, creaturePtr repriser, creaturePtr target, int dam, DamageType dt);
	void combat_kill(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);


	//	Effects
	void affliction_get(gamedataPtr gdata, creaturePtr target, const Affliction af);
	void affliction_resist(gamedataPtr gdata, creaturePtr target, const Affliction af);
	void energy_damage(gamedataPtr gdata, creaturePtr target, const DamageType dt, const int amt);
	void drowning(gamedataPtr gdata, creaturePtr target);
	void agony(gamedataPtr gdata, creaturePtr target);


	//	AI actions
	void ai_puke(gamedataPtr gdata, creaturePtr t);
}