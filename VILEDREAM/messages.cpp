#include "messages.h"


void messages::add(gamedataPtr gdata, string txt, colorType col)
{
	gdata->_messages.insert(gdata->_messages.begin(), message(txt, col));
	if (gdata->_messages.size() > MESSAGE_COUNT_MAX)
		gdata->_messages.pop_back();
}

void messages::combat_hit(gamedataPtr gdata, creaturePtr attacker, creaturePtr target, int dmg, bool crit)
{
	if (attacker->isPlayer())
	{
		if (crit)
			messages::add(gdata, "CRITICALLY HIT " + target->getName() + " for *" + to_string(dmg) + "* dmg!!", COLOR_POSITIVE);
		else
			messages::add(gdata, "Hit " + target->getName() + " for " + to_string(dmg) + " dmg!", COLOR_HIGHLIGHT_POS);
	}
	else if (target->isPlayer())
		messages::add(gdata, ' ' + attacker->getName() + " hit you for " + to_string(dmg) + " dmg!", COLOR_WARNING);
}

void messages::combat_touch(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	if (attacker->isPlayer())
		messages::add(gdata, "You touch " + target->getName() + "!", COLOR_HIGHLIGHT_POS);
	else if (target->isPlayer())
		messages::add(gdata, ' ' + attacker->getName() + " touches you!", COLOR_WARNING);
}

void messages::combat_miss(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	if (attacker->isPlayer())
		messages::add(gdata, "You missed " + target->getName() + "!", COLOR_HIGHLIGHT_NEG);
	else if (target->isPlayer())
		messages::add(gdata, ' ' + attacker->getName() + " attacks you and misses!", COLOR_MEDIUM);
}

void messages::combat_block(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	if (attacker->isPlayer())
		messages::add(gdata, target->getName() + " blocks you!", COLOR_HIGHLIGHT_NEG);
	else if (target->isPlayer())
		messages::add(gdata, "You block " + attacker->getName() + "!", COLOR_MEDIUM);
}

void messages::combat_riposte(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	if (attacker->isPlayer())
		messages::add(gdata, "You riposte!", COLOR_POSITIVE);
	else if (target->isPlayer())
		messages::add(gdata, ' ' + attacker->getName() + " ripostes!", COLOR_HIGHLIGHT_NEG);
}

void messages::combat_reprisal(gamedataPtr gdata, creaturePtr repriser, creaturePtr target, int dam, DamageType dt)
{
	if (repriser->isPlayer())
		messages::add(gdata, "You retaliate and inflict " + to_string(dam) + " " + getDamageTypeName(dt) + " damage!", COLOR_HIGHLIGHT_POS);
	else if (target->isPlayer())
		messages::add(gdata, ' ' + target->getName() + " retaliates for " + to_string(dam) + " " + getDamageTypeName(dt) + " damage!", COLOR_HIGHLIGHT_NEG);
}

void messages::combat_kill(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	if (attacker->isPlayer())
		messages::add(gdata, " % Killed " + target->getName() + "! %", COLOR_POSITIVE);
}

void messages::affliction_get(gamedataPtr gdata, creaturePtr target, const Affliction af)
{
	if (target->isPlayer())
		messages::add(gdata, "You gained the " + getAfflictionName(af) + " affliction!", getAfflictionColor(af));
	else if (!target->hasAffliction(af))
		messages::add(gdata, ' ' + target->getName() + " gained affliction " + getAfflictionName(af) + "!", COLOR_HIGHLIGHT_POS);
}

void messages::affliction_resist(gamedataPtr gdata, creaturePtr target, const Affliction af)
{
	if (target->isPlayer())
		messages::add(gdata, "You resist " + getAfflictionName(af) + ".", COLOR_HIGHLIGHT_POS);
}

void messages::energy_damage(gamedataPtr gdata, creaturePtr target, const DamageType dt, const int amt)
{
	if (target->isPlayer())
		messages::add(gdata, "You take " + to_string(amt) + " " + getDamageTypeName(dt) + " damage!", getDamageTypeColor(dt));
}

void messages::drowning(gamedataPtr gdata, creaturePtr target)
{
	if (target->isPlayer())
		messages::add(gdata, "You are drowning!", COLOR_NEGATIVE);
}

void messages::agony(gamedataPtr gdata, creaturePtr target)
{
	if (target->isPlayer())
		messages::add(gdata, "You writhe in agony!", COLOR_NEGATIVE);
}

void messages::ai_puke(gamedataPtr gdata, creaturePtr t)
{
	if (!t->isPlayer() && isCreatureInVisibleMonsterList(gdata, t))
		messages::add(gdata, t->getName() + " vomits profusely!", COLOR_MEDIUM);
}
