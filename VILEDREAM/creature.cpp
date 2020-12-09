#include "creature.h"


creature::creature() : _damageTaken(0)
{
	for (unsigned i = 0; i < AFFL__NONE; i++)
		_afflictions.push_back(0);
	for (unsigned i = 0; i < BUFF__NONE; i++)
		_buffs.push_back(0);
	for (unsigned i = 0; i < COND__NONE; i++)
		_conditions.push_back(0);
	for (unsigned i = 0; i < RESIST__NONE; i++)
		_resistances.push_back(0);
	_movedLastTurn = false;
}

//	Health left out of 100
int creature::getHealthPercent() const
{
	return 100.0f - 100.0f * (float)_damageTaken / (float)getMaxHealth();
}

void creature::healDamage(int amt)
{
	_damageTaken = MAX(0, _damageTaken - amt);
}

void creature::addAfflictionStacks(const Affliction af, int stacks)
{
	if (stacks > _afflictions[af])
		_afflictions[af] = stacks;
}


//	Reduce durations of afflictions that are subject to reduction
void creature::tickAfflictions()
{
	for (unsigned i = 0; i < _afflictions.size(); i++)
	{
		if (_afflictions[i] > 0)
			_afflictions[i] -= 1;
	}
}


//	Where 'addBuff' replaces the current duration, this one actually stacks it
//	(so if we have 5 stacks and call this with duration=10, for example, we will end up with duration 15)
//	'upTo' is the maximum we will stack to; if it's set to -1, max is unlimited.
void creature::stackBuff(const BuffType bf, int duration, int upTo)
{
	_buffs[bf] += duration;
	if (upTo != -1)
		_buffs[bf] = MIN(_buffs[bf], upTo);
}

//	If we already have this buff, uses the higher of the two durations.
void creature::addBuff(const BuffType bf, int duration)
{
	if (_buffs[bf] < duration)
		_buffs[bf] = duration;
}

void creature::tickBuffs()
{
	for (unsigned i = 0; i < _buffs.size(); i++)
	{
		if (_buffs[i] > 0)
			_buffs[i] -= 1;
	}
}

void creature::addCondition(const Condition cond, int duration)
{
	if (duration > _conditions[cond])
		_conditions[cond] = duration;
}

void creature::tickConditions()
{
	for (unsigned i = 0; i < _conditions.size(); i++)
		_conditions[i] -= 1;
}

int creature::getMeleeAccuracy() const
{
	int total = getBaseAccuracy();
	if (knowsFeat(FEAT_DUALSTRIKE))
		total -= 4;
	return total;
}


//	Added to melee damage rolls
int creature::getMeleeDamageBonus() const
{
	int total = getBaseDamageBonus();

	if (hasBuff(BUFF_MIGHT))
		total += total / 2 + 4;
	
	if (hasBuff(BUFF_RAMPAGE))
		total += 1 + getBuffDuration(BUFF_RAMPAGE) / 10;
	
	return total;
}


//	No. of melee attacks we make per turn
int creature::getMeleeAttackCount() const
{
	int total = 1;
	if (hasMeleeWeaponTag(WTAG_DOUBLE))
		total++;
	if (knowsFeat(FEAT_DUALSTRIKE))
		total++;
	return total;
}


//	Additional damage inflicted by our melee attacks, of the given type.
//	Returns a 0,0 roll if none is inflicted.
intpair creature::getMeleeSpecialDamage(const DamageType dt) const
{
	return intpair(0, 0);
}


int creature::getResistance(const ResistanceType res) const
{
	//	NO RESISTANCE
	if (res == RESIST__NONE)
		return 0;

	//	base value
	int total = _resistances[res];


	//	modifiers from conditions/etc
	switch (res)
	{
	case(RESIST_COLD):
		if (hasCondition(COND_CHILLED))		total -= 25;
		if (hasCondition(COND_HEATED))		total += 25;
		break;

	case(RESIST_FIRE):
		if (hasCondition(COND_CHILLED))		total += 25;
		if (hasCondition(COND_HEATED))		total -= 25;
		if (hasCondition(COND_OILY))		total -= 50;
		if (hasCondition(COND_SOAKED))		total += 50;
		break;
		
	case(RESIST_ELECTRIC):
		if (hasCondition(COND_OILY))		total += 50;
		if (hasCondition(COND_SOAKED))		total -= 25;
		break;


	case(RESIST_BLEED):
		if (hasBuff(BUFF_STONESKIN))		total += 50;
		break;
	}

	return total;
}


//	Checks whether our grappler is still valid.
//	A grappler is invalid if they are no longer adjacent, are dead, unconscious, etc.
//	If the grappler is no longer valid, sets it to nullptr.
void creature::verifyGrappler()
{
	if (_grappler != nullptr)
	{
		if (_grappler->isDead() ||
			_grappler->hasAffliction(AFFL_FEAR) ||
			hypot(_pos.first - _grappler->_pos.first, _pos.second - _grappler->_pos.second) >= 2)
		{
			_grappler = nullptr;
		}
	}
}


int creature::getMoveActionDelay() const
{
	auto del = ACTION_DELAY_NORMAL;
	if (hasBuff(BUFF_HASTE))
		del = adjustActionDelay(del, 1);
	if (hasBuff(BUFF_STONESKIN))
		del = adjustActionDelay(del, -1);
	return del;
}

int creature::getAttackActionDelay() const
{
	auto del = ACTION_DELAY_NORMAL;
	if (hasBuff(BUFF_HASTE))
		del = adjustActionDelay(del, 1);
	return del;
}

void creature::setActionDelayTo(const int val)
{
	_actionDelay = val;
	_movedLastTurn = false;
}

void creature::tickActionDelay()
{
	if (_actionDelay > 0)
		_actionDelay--;
}


//	A positive 'adj' returns a delay that's one step faster; a negative 'adj' returns a delay that's one step slower.
//	An 'adj' of zero returns the given base_delay unmodified.
//	Capped at the maximum & minimum action delays.
const int adjustActionDelay(const int base_delay, const int adj)
{
	if (adj > 0)
		return MAX(ACTION_DELAY_FAST, base_delay / 2);
	else if (adj < 0)
		return MIN(ACTION_DELAY_SLOW, base_delay * 2);
	else
		return base_delay;
}
