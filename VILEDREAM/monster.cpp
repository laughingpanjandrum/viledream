#include "monster.h"

monster::monster() : creature()
{
	//	default stat values
	_accMelee = 0;
	_accRanged = 0;
	_evasion = 8;
	_damageResist = 0;
	_blockValue = 0;
	_maxHealth = 1;

	_meleeDmgBonus = 0;

	_meleeSpecialDamageType = DTYPE__MAX;
}

monster::monster(const vector<string> tokens) : monster()
{
	for (unsigned i = 0; i < tokens.size(); i += 2)
		setAttribute(tokens[i], tokens[i + 1]);
}

int monster::getMeleeAccuracy() const
{
	return creature::getMeleeAccuracy() + _accMelee;
}

int monster::getRangedAccuracy() const
{
	return _accRanged;
}

int monster::getResistance(const ResistanceType res) const
{
	int total = 0;

	//	construct resistances
	if (hasFlag("construct"))
	{
		switch (res)
		{
		case(RESIST_AGONY):
		case(RESIST_BLEED):
		case(RESIST_FEAR):
		case(RESIST_NECROTIC):
		case(RESIST_POISON):
			total += 100;
			break;
		}
	}

	//	undead resistances
	else if (hasFlag("undead"))
	{
		switch (res)
		{
		case(RESIST_AGONY):
		case(RESIST_NECROTIC):
		case(RESIST_POISON):
			total += 100;
			break;
		}
	}

	//	vulnerabilities
	switch (res)
	{
	case(RESIST_FIRE):
		if (hasFlag("vulnerable_fire"))	total -= 50;
		break;
	}

	return total;
}

bool monster::hasMeleeWeaponTag(const WeaponTag wtag) const
{
	switch (wtag)
	{
	case(WTAG_DROWNING):		return hasFlag("drowning_attack");
	case(WTAG_REACH):			return hasFlag("melee_reach");
	default:
		return false;
	}
}

intpair monster::getMeleeSpecialDamage(const DamageType dt) const
{
	if (dt == _meleeSpecialDamageType)
		return _meleeSpecialDamage;
	else
		return intpair(0, 0);
}

int monster::getMeleeAfflictionStacks(const Affliction af) const
{
	switch (af)
	{
	case(AFFL_BLEED):
		if (hasFlag("melee_bleed_attack"))	return 1 + dieRoll(_dangerLevel, 3);
		break;
	}
	return 0;
}

bool monster::immuneToDrowning() const
{
	return hasFlag("amphibious") || hasFlag("construct");
}

int monster::getMoveActionDelay() const
{
	if (hasFlag("moves_quickly"))
		return ACTION_DELAY_FAST;
	else if (hasFlag("moves_slowly"))
		return ACTION_DELAY_SLOW;
	else
		return ACTION_DELAY_NORMAL;
}


//	Translates a string of the form "xdy" into an intpair with value x,y
intpair monster::dieFromString(const string val)
{
	auto v = splitLine(val, 'd');
	return intpair(stoi(v.first), stoi(v.second));
}

//	Set an attribute given the name of the value (tag) & its value (val)
void monster::setAttribute(const string tag, const string val)
{
	//	Flavour
	if (tag == "name")
		_name = val;
	else if (tag == "color")
		_color = translateColour(val);
	else if (tag == "glyph")
		_glyph = stoi(val);


	//	Arrangement
	else if (tag == "id")
		_id = val;
	else if (tag == "dl")
		_dangerLevel = stoi(val);


	//	Vitals
	else if (tag == "health")
		_maxHealth = stoi(val);
	else if (tag == "flag")
		_flags.push_back(val);


	//	Attacks
	else if (tag == "melee_acc")
		_accMelee = stoi(val);
	else if (tag == "ranged_acc")
		_accRanged = stoi(val);
	else if (tag == "melee_dmg")
		_dmgMelee = dieFromString(val);
	else if (tag == "ranged_dmg")
		_dmgRanged = dieFromString(val);
	else if (tag == "melee_dmg_bonus")
		_meleeDmgBonus = stoi(val);


	//	Special attacks
	else if (tag == "melee_fire_dmg")
		setMeleeSpecialDamage(DTYPE_FIRE, dieFromString(val));


	//	Defences
	else if (tag == "ev")
		_evasion = stoi(val);
	else if (tag == "dr")
		_damageResist = stoi(val);
	else if (tag == "block")
		_blockValue = stoi(val);


	//	Other unique abilities
	else if (tag == "spawn_on_death")
		_spawnsOnDeath = val;
	else if (tag == "spawns")
		_spawns = val;


	//	PROBLEM???
	else
		cout << "Monster lacks attribute with tag " << tag << ", given value " << val << endl;
}

void monster::setMeleeSpecialDamage(const DamageType dt, const intpair die)
{
	_meleeSpecialDamage = die;
	_meleeSpecialDamageType = dt;
}
