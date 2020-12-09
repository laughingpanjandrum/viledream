#include "combat.h"


//	Returns true if the first pair of surfaces s1,s2 matches the second pair t1,t2, in any order
bool testForSurfacePair(const surface s1, const surface s2, const surface t1, const surface t2)
{
	return (s1 == t1 && s2 == t2) || (s1 == t2 && s2 == t1);
}


//	Returns what surface results if you attempt to place 'sf' on 'base'.
surface getSurfaceCollision(const surface base, const surface sf)
{
	//	if there's no surface on the point, just slap the new one down!
	if (base == SURFACE__NONE)
		return sf;

	//	chilled water turns to ice
	else if (base == SURFACE_WATER && sf == SURFACE_FROST)
		return SURFACE_ICE;

	//	corpses can only replace select tiles
	else if (sf == SURFACE_CORPSE && !(base == SURFACE_BLOOD || base == SURFACE_FROST))
		return base;

	//	fire and oil make superfire
	else if (testForSurfacePair(base, sf, SURFACE_FIRE, SURFACE_OIL))
		return SURFACE_BURNING_OIL;

	//	throwing more oil onto a fire doesn't turn it into oil!
	else if (testForSurfacePair(base, sf, SURFACE_BURNING_OIL, SURFACE_OIL))
		return SURFACE_BURNING_OIL;

	//	fire melts ice
	else if (testForSurfacePair(base, sf, SURFACE_ICE, SURFACE_FIRE))
		return SURFACE_WATER;

	//	oil and water don't mix
	else if (testForSurfacePair(base, sf, SURFACE_OIL, SURFACE_WATER))
		return base;

	//	oil is unfreezable
	else if (base == SURFACE_OIL && (sf == SURFACE_FROST || sf == SURFACE_ICE))
		return base;

	//	electrified water won't be changed into just elec
	else if (base == SURFACE_WATER_ELEC && sf == SURFACE_ELECTRIC)
		return SURFACE_WATER_ELEC;

	//	water gets ELECTRIFIED
	else if (testForSurfacePair(base, sf, SURFACE_WATER, SURFACE_ELECTRIC))
		return SURFACE_WATER_ELEC;


	//	the new surface wins by default
	return sf;
}


//	Returns what surface results if you try to apply it to the given tile.
surface getSurfaceAndTileCollision(const maptile tl, const surface sf)
{
	//	Various water effects
	if (tl == TILE_WATER_DEEP)
	{
		//	Water doesn't stack!
		if (sf == SURFACE_WATER)
			return SURFACE__NONE;

		//	Freezing!
		else if (sf == SURFACE_FROST)
			return SURFACE_ICE;

		//	Become ELECTRICITY
		else if (sf == SURFACE_ELECTRIC)
			return SURFACE_WATER_ELEC;
	}

	//	No change by default
	return sf;
}


//	Attempt to add a surface at a given point.
//	Different interactions of tiles/surfaces can produce different results.
void tryAddSurface(gamedataPtr gdata, surface sf, intpair pt)
{
	auto tl = gdata->_map->getTile(pt);
	if (canCoverWithSurface(tl, sf))
	{
		sf = getSurfaceCollision(gdata->_map->getSurface(pt), sf);
		sf = getSurfaceAndTileCollision(tl, sf);
		gdata->_map->setSurface(sf, pt);
	}
}


//	Applies a surface to an entire square region.
void applySurfaceToRegion(gamedataPtr gdata, surface sf, const intpair ctr, const int sz)
{
	for (int x = ctr.first - sz; x <= ctr.first + sz; x++)
	{
		for (int y = ctr.second - sz; y <= ctr.second + sz; y++)
		{
			if (gdata->_map->inBounds(x, y))
				tryAddSurface(gdata, sf, intpair(x, y));
		}
	}
}


//	Returns true if the attacker can inflict a drowning attack on the victim.
//	One of them must be standing in water for this to work.
bool creatureSubjectToDrowning(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	for (auto cr : { attacker, target })
	{
		if (gdata->_map->getTile(cr->_pos) == TILE_WATER_DEEP)
			return true;
		auto sf = gdata->_map->getSurface(cr->_pos);
		if (sf == SURFACE_WATER || sf == SURFACE_WATER_ELEC)
			return true;
	}
	return false;
}


//	This tells the game to spawn a monster with the given id at the given location as soon as possible.
//	If 'with_damage' is not zero, the new monster will spawn with the given amount of damage taken, instead of its default amount.
void queueMonsterSpawn(gamedataPtr gdata, const string id, const intpair pt, const int with_damage)
{
	gdata->_triggerMonsterSpawnId.push_back(id);
	gdata->_triggerMonsterSpawnPt.push_back(pt);
	gdata->_triggerMonsterSpawnDamage.push_back(with_damage);
}


//	Monster splits off another copy of itself when it takes damage.
//	The new monster has less health than the original.
void splitOnTakeDamage(gamedataPtr gdata, monsterPtr mon)
{
	auto pts = getAdjacentFreePoints(gdata, mon, mon->_pos);
	if (!pts.empty())
	{
		auto pt = pts[randrange(pts.size())];
		auto dam = mon->_damageTaken + 1;
		if (dam < mon->getMaxHealth())
			queueMonsterSpawn(gdata, mon->_id, pt, dam);
	}
}


//	'Reprisal' attacks are triggered when the target successfuly damages the repriser with a melee attack.
//	This checks to see if the repriser has any such attacks available, and if so, executes them.
//	Reprisal attacks use the attacker's unmodified base accuracy.
void doReprisalAttacks(gamedataPtr gdata, creaturePtr repriser, creaturePtr target)
{
	auto acc = repriser->getBaseAccuracy();
	for (auto atk : repriser->getReprisalAttacks())
	{
		//	roll against EV
		if (!rollToEvade(gdata, acc, target))
		{
			//	roll damage
			auto dam = dieRoll(atk._damage.first, atk._damage.second);
			if (atk._dtype == DTYPE_PHYSICAL)
			{
				dam = applyDamageResist(gdata, target, dam);
				creatureTakeDamage(gdata, target, dam);
			}
			else
			{
				inflictEnergyDamage(gdata, target, atk._dtype, dam);
			}
			messages::combat_reprisal(gdata, repriser, target, dam, atk._dtype);
		}
	}
}


//	Stuff the player gets when a monster dies: xp, healing surges, etc
void getMonsterKillRewards(gamedataPtr gdata, monsterPtr mon)
{
	//	healing surges & kill xp
	gdata->_player->gainHealingSurgeProgress();
	addKillXP(gdata, mon->_dangerLevel);


	//	cumulative damage buff on kills
	if (gdata->_player->hasRingOfType(itemData::RING_RAMPAGE))
		gdata->_player->stackBuff(BUFF_RAMPAGE, 10, 50);
}


//	Special things monsters can do when they die.
void doMonsterDeathEffects(gamedataPtr gdata, monsterPtr mon)
{
	//	drop a corpse
	if (!mon->hasFlag("no_corpse"))
		tryAddSurface(gdata, SURFACE_CORPSE, mon->_pos);

	//	death spawns
	auto spawn_id = mon->spawnsOnDeath();
	if (!spawn_id.empty())
		queueMonsterSpawn(gdata, spawn_id, mon->_pos);

	//	nauseating explosion
	if (mon->hasFlag("nauseating"))
	{
		for (auto t : getAdjacentCreatures(gdata, mon->_pos))
			tryAddAffliction(gdata, t, AFFL_NAUSEA, dieRoll(mon->_dangerLevel, 4));
	}
}

//	Target dies & is removed from the map.
void killCreature(gamedataPtr gdata, creaturePtr target)
{
	if (!target->isPlayer())
	{
		auto mon = static_pointer_cast<monster>(target);

		//	removed from map
		gdata->_map->removeCreature(target);

		//	benefits to the player & special monster effects
		doMonsterDeathEffects(gdata, mon);
		getMonsterKillRewards(gdata, mon);
	}
}


//	Inflict damage directly. Any damage resistance should have been applied already.
void creatureTakeDamage(gamedataPtr gdata, creaturePtr target, int amt)
{
	//	Electrification increases damage taken
	if (target->hasAffliction(AFFL_ELECTRIFY))
	{
		int bns = 1 + (float)amt * (float)target->getAfflictionStacks(AFFL_ELECTRIFY) * 0.1f;
		//cout << " Electrification modified " << amt << " damage by " << bns << endl;
		amt += bns;
	}

	//	lose health
	target->takeDamage(amt);

	//	check for death
	if (target->isDead())
		killCreature(gdata, target);
}


//	Suffered if we try to move or attack while affected by Agony.
void inflictAgonyDamage(gamedataPtr gdata, creaturePtr target)
{
	messages::agony(gdata, target);
	creatureTakeDamage(gdata, target, randint(1, 6));
}


//	Inflicts a percentage of max health each turn.
void inflictDrowning(gamedataPtr gdata, creaturePtr target)
{
	auto dam = MAX(1, target->getMaxHealth() / 10);
	creatureTakeDamage(gdata, target, dam);
	messages::drowning(gdata, target);
}


//	Target takes some non-physical damage. Applies any damage resistance we possess first.
//	Does NOT check block or evade!
void inflictEnergyDamage(gamedataPtr gdata, creaturePtr target, const DamageType dt, int amt)
{
	//	check resistance
	auto rval = target->getResistance(getResistanceForDamageType(dt));
	if (rval != 0)
		amt -= (float)rval * (float)amt / 100.0f;


	//	apply remaining damage, if any
	if (amt > 0)
	{
		//	the damage
		creatureTakeDamage(gdata, target, amt);
		addAnimation(gdata, anim_FlashGlyph(target->_pos, '%', getDamageTypeColor(dt)));
		messages::energy_damage(gdata, target, dt, amt);

		//	some damage types have associated afflictions
		int af_stacks = 2 + amt / 3;
		switch (dt)
		{
		case(DTYPE_COLD):		tryAddAffliction(gdata, target, AFFL_FREEZE, af_stacks); break;
		case(DTYPE_ELECTRIC):	tryAddAffliction(gdata, target, AFFL_ELECTRIFY, af_stacks * 2); break;
		case(DTYPE_FIRE):		tryAddAffliction(gdata, target, AFFL_BURN, af_stacks); break;
		case(DTYPE_NECROTIC):	tryAddAffliction(gdata, target, AFFL_DRAIN, af_stacks); break;
		}
	}
}



//	Attempts to confer an affliction upon the given target. They have a chance to resist.
void tryAddAffliction(gamedataPtr gdata, creaturePtr target, Affliction af, int stacks)
{
	//	check resistance
	int rval = target->getResistance(getResistanceForAffliction(af));

	//	Resist?
	if (rval > 0 && roll_percent(rval))
	{
		messages::affliction_resist(gdata, target, af);
	}

	//	Failed to resist
	else
	{
		//	negative resistance may make it worse
		if (rval < 0 && roll_percent(-rval))
			stacks += randint(1, stacks);

		//	apply the affliction
		messages::affliction_get(gdata, target, af);
		target->addAfflictionStacks(af, stacks);
	}
}


//	Target rolls to evade an attack with given accuracy.
//	Returns True if evaded successfully.
bool rollToEvade(gamedataPtr gdata, int acc, creaturePtr target)
{
	return dieRoll(2, 10) + acc < target->getEvasion();
}



//	Target rolls to block an attack with given accuracy.
//	Returns True if blocked successfully.
//	We must have a non-zero block value to be able to block.
bool rollToBlock(gamedataPtr gdata, int acc, creaturePtr target)
{
	int blk = target->getBlockValue();
	if (blk > 0)
		return dieRoll(2, 10) + acc < target->getBlockValue();
	else
		return false;
}


//	Target applies their damage resistance to the given damage amount.
//	Returns the remaining damage.
int applyDamageResist(gamedataPtr gdata, creaturePtr target, int dam)
{
	int dr = target->getDamageResist();
	int dmg_reduce = randint(dr / 2, dr);
	dam = MAX(0, dam - dmg_reduce);
	if (dam == 0 && roll_percent(100 - dr))
		dam = 1;
	return dam;
}


//	Additional effects applied on-hit with a melee attack.
void applyMeleeHitEffects(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	//	special damage types
	for (auto dt : SPECIAL_DAMAGE_TYPES)
	{
		auto ddie = attacker->getMeleeSpecialDamage(dt);
		if (ddie.first > 0)
			inflictEnergyDamage(gdata, target, dt, dieRoll(ddie.first, ddie.second));
	}

	//	additional afflictions
	for (unsigned i = 0; i < AFFL__NONE; i++)
	{
		auto af = static_cast<Affliction>(i);
		auto af_stacks = attacker->getMeleeAfflictionStacks(af);
		if (af_stacks > 0)
			tryAddAffliction(gdata, target, af, af_stacks);
	}

	//	grappling
	if (attacker->grapplesOnHit())
		target->setGrappler(attacker);

	//	from flags
	if (attacker->hasMeleeWeaponTag(WTAG_DROWNING) && creatureSubjectToDrowning(gdata, attacker, target))
		inflictDrowning(gdata, target);
	if (attacker->hasMeleeWeaponTag(WTAG_STAGGER))
	{
		int stag = randint(1, 3);
		if (attacker->knowsFeat(FEAT_MACE_PROFICIENCY))
			stag *= 2;
		tryAddAffliction(gdata, target, AFFL_STAGGER, stag);
	}
}



//	Additional effects applied on-hit with a ranged attack.
void applyRangedHitEffects(gamedataPtr gdata, creaturePtr attacker, creaturePtr target)
{
	applyArrowEffect(gdata, target, attacker->getArrowTypeUsed());
	if (attacker->knowsFeat(FEAT_PUSHING_SHOT) && roll_percent(30))
		tryAddAffliction(gdata, target, AFFL_STAGGER, randint(2, 3));
}


//	if 'is_melee' is false, we assume it's a ranged attack (& alter stats accordingly)
//	set 'allow_riposte' to false to prevent chain-ripostes
//	'accAdjust' is just a flat modifier to attack accuracy from outside factors, eg a lunge
void triggerWeaponAttack(gamedataPtr gdata, creaturePtr attacker, creaturePtr target, bool is_melee, bool is_riposte, int accAdjust, int dmgAdjust)
{
	//	get attacker properties
	int acc, dbonus, crit_chance, crit_mult;
	intpair dmg_dice;
	if (is_melee)
	{
		acc = attacker->getMeleeAccuracy();
		dbonus = attacker->getMeleeDamageBonus();
		dmg_dice = attacker->getMeleeDamageDice();
		crit_chance = attacker->getMeleeCritChance();
		crit_mult = attacker->getMeleeCritMultiplier();
	}
	else
	{
		acc = attacker->getRangedAccuracy();
		dbonus = attacker->getRangedDamageBonus();
		dmg_dice = attacker->getRangedDamageDice();
		crit_chance = 5;
		crit_mult = 2;
	}
	acc += accAdjust;
	dbonus += dmgAdjust;


	//	ranged attacks have acc penalties if used at point-blank or too-long range
	auto dist = getDistanceBetweenCreatures(attacker, target);
	if (!is_melee)
	{
		auto attackRange = attacker->getAttackRange();
		if (dist < 2)
			acc -= 4;
		else if (dist > attackRange)
			acc -= dist - attackRange;
	}


	//	riposte bonuses
	if (is_riposte && attacker->knowsFeat(FEAT_BLADE_PROFICIENCY))
	{
		acc += 2;
		dbonus += 2;
	}


	//	check for multiple attacks
	int atk_count = 1;
	if (is_melee) atk_count = attacker->getMeleeAttackCount();

	//	do all the attacks
	while (atk_count-- > 0)
	{
		//	now we can roll the attack
		if (!rollToEvade(gdata, acc, target))
		{
			//	if target failed to dodge, they can still try to block
			if (rollToBlock(gdata, acc, target))
			{
				//	blocked, haha
				messages::combat_block(gdata, attacker, target);
				addAnimation(gdata, anim_FlashGlyph(target->_pos, 4, COLOR_WHITE));

				//	can target riposte on a miss?
				if (is_melee && !is_riposte && target->canRiposte())
					triggerWeaponAttack(gdata, target, attacker, true, true);
			}

			//	it's a hit!!
			else
			{
				//	check for crit
				bool crit = roll_percent(crit_chance);
				if (crit)
					dbonus += dmg_dice.first * dmg_dice.second * crit_mult;

				//	now roll damage
				int dam = dieRoll(dmg_dice.first, dmg_dice.second) + dbonus;

				//	apply DR
				dam = applyDamageResist(gdata, target, dam);
				messages::combat_hit(gdata, attacker, target, dam, crit);


				//	INFLICT
				if (dam > 0)
				{
					//	actually apply the damage
					addAnimation(gdata, anim_FlashGlyph(target->_pos, getGlyphForVector(attacker->_pos, target->_pos), TCODColor::crimson));
					creatureTakeDamage(gdata, target, dam);

					//	chance to bleed
					if (roll_one_in(2))
						tryAddSurface(gdata, SURFACE_BLOOD, target->_pos);


					//	check for special effects from melee
					if (is_melee)
						applyMeleeHitEffects(gdata, attacker, target);

					//	& from ranged attacks
					else
						applyRangedHitEffects(gdata, attacker, target);


					//	check for death
					if (target->isDead())
						messages::combat_kill(gdata, attacker, target);

					//	additional special effects
					else
					{
						if (target->splitOnTakeDamage())
							splitOnTakeDamage(gdata, static_pointer_cast<monster>(target));
						if (is_melee)
							doReprisalAttacks(gdata, target, attacker);
					}
				}
			}
		}
		else
		{
			//	hah you missed, idiot
			messages::combat_miss(gdata, attacker, target);

			//	can target riposte on a miss?
			if (is_melee && !is_riposte && target->canRiposte())
			{
				messages::combat_riposte(gdata, target, attacker);
				triggerWeaponAttack(gdata, target, attacker, true, true);
			}
		}

		//	stop if target dies
		if (target->isDead())
			break;
	}

	//	attacker's action delay is set regardless of outcome (unless this was a riposte attack, those are free)
	if (!is_riposte)
	{
		attacker->setActionDelayTo(attacker->getAttackActionDelay());
		if (attacker->hasAffliction(AFFL_AGONY))
			inflictAgonyDamage(gdata, attacker);
	}
}


//	Call this AFTER the move has been executed.
void checkForLungeAttack(gamedataPtr gdata, creaturePtr mover, intpair vec)
{
	auto pt = mover->_pos;
	intpair_add(&pt, &vec);
	if (gdata->_map->inBounds(pt))
	{
		auto t = gdata->_map->getCreature(pt);
		if (t != nullptr)
			triggerWeaponAttack(gdata, mover, t, true, true, 3);
	}
}


//	Attacks ALL adjacent enemies.
void triggerCleaveAttack(gamedataPtr gdata, creaturePtr attacker)
{
	auto ctr = attacker->_pos;

	//	sum up damage bonus, if necessary
	int dbonus = 0;
	if (attacker->knowsFeat(FEAT_AXE_PROFICIENCY))
		dbonus = getAdjacentCreatures(gdata, ctr).size();


	//	hit everything adjacent
	for (int x = ctr.first - 1; x <= ctr.first + 1; x++)
	{
		for (int y = ctr.second - 1; y <= ctr.second + 1; y++)
		{
			if (gdata->_map->inBounds(x, y) && !(x == ctr.first && y == ctr.second))
			{
				auto t = gdata->_map->getCreature(x, y);
				if (t != nullptr)
					triggerWeaponAttack(gdata, attacker, t, true, false, 0, dbonus);
			}
		}
	}
}


//	Checks to see if we CAN do a reach attack, & if so, attempts one in the given direction.
void triggerReachAttack(gamedataPtr gdata, creaturePtr attacker, intpair vec)
{
	//	check to see what our reach is
	int reach = 1;
	if (attacker->hasMeleeWeaponTag(WTAG_REACH))
		reach = 2;
	else if (attacker->hasMeleeWeaponTag(WTAG_WHIP))
		reach = 5;

	//	attack along that vector
	auto pt = attacker->_pos;
	for (unsigned i = 0; i < reach; i++)
	{
		intpair_add(&pt, &vec);
		if (gdata->_map->inBounds(pt))
		{
			//	check for a target
			auto t = gdata->_map->getCreature(pt);
			if (t != nullptr)
			{
				int acc_adjust = (attacker->knowsFeat(FEAT_SPEAR_PROFICIENCY)) ? 2 : 0;
				triggerWeaponAttack(gdata, attacker, t, true, false, acc_adjust);
				break;
			}

			//	if we hit a wall, stop
			if (!gdata->_map->isWalkable(pt))
				break;
		}

		//	if we go out of map bounds, stop
		else
			break;
	}
}


//	Attacker tries to touch the victim; on hit, applies the given affliction.
//	Different from a weapon attack b/c no weapon is used.
void triggerTouchAttack(gamedataPtr gdata, creaturePtr attacker, creaturePtr target, const Affliction af, const int stacks)
{
	auto acc = attacker->getMeleeAccuracy();
	auto ev = target->getEvasion();
	auto roll = dieRoll(2, 10);
	if (roll + acc >= ev)
	{
		//	now we take the affliction
		messages::combat_touch(gdata, attacker, target);
		tryAddAffliction(gdata, target, af, stacks);
	}
	else
		messages::combat_miss(gdata, attacker, target);
	attacker->setActionDelayTo(attacker->getAttackActionDelay());
}


//	Apply special arrow effect, if any.
void applyArrowEffect(gamedataPtr gdata, creaturePtr target, const itemData::itid id)
{
	switch (id)
	{
	case(itemData::ARROWS_POISON):
		tryAddAffliction(gdata, target, AFFL_POISON, 5);
		break;

	case(itemData::ARROWS_SERRATED):
		tryAddAffliction(gdata, target, AFFL_BLEED, dieRoll(2, 6));
		break;

	case(itemData::ARROWS_SILVERED):
		if (target->isUndead())
			creatureTakeDamage(gdata, target, dieRoll(2, 6));
		break;
	}
}



//	Apply the effects of a thrown weapon with the given id to a single point.
void applyThrownWeaponEffect(gamedataPtr gdata, itemData::itid id, intpair pt)
{
	auto t = gdata->_map->getCreature(pt);
	switch (id)
	{
	case(itemData::URN_FIRE):
		if (t != nullptr) inflictEnergyDamage(gdata, t, DTYPE_FIRE, dieRoll(2, 6));
		tryAddSurface(gdata, SURFACE_FIRE, pt); 
		break;

	case(itemData::URN_ICE):
		if (t != nullptr) inflictEnergyDamage(gdata, t, DTYPE_COLD, dieRoll(2, 6));
		tryAddSurface(gdata, SURFACE_FROST, pt);
		break;

	case(itemData::URN_LIGHTNING):
		if (t != nullptr) inflictEnergyDamage(gdata, t, DTYPE_ELECTRIC, dieRoll(2, 6));
		tryAddSurface(gdata, SURFACE_ELECTRIC, pt);
		break;

	case(itemData::URN_OIL):	tryAddSurface(gdata, SURFACE_OIL, pt); break;
	case(itemData::URN_WATER):	tryAddSurface(gdata, SURFACE_WATER, pt); break;
	}
}


//	A thrown weapon strikes a target point.
void hitPointWithThrownWeapon(gamedataPtr gdata, itemPtr thrown, intpair pt)
{
	int r = 2;
	for (int x = pt.first - r; x <= pt.first + r; x++)
	{
		for (int y = pt.second - r; y <= pt.second + r; y++)
		{
			if (gdata->_map->inBounds(x, y))
				applyThrownWeaponEffect(gdata, thrown->_id, intpair(x, y));
		}
	}
	addAnimation(gdata, anim_Explosion(pt, r, '#', thrown->getColor()));
}


//	Use a healing surge, if we have one.
void playerUseHealingSurge(gamedataPtr gdata)
{
	if (gdata->_player->hasHealingSurge())
	{
		gdata->_player->useHealingSurge();
		messages::add(gdata, "You feel better!", COLOR_POSITIVE);
		gdata->_player->setActionDelay();
	}
	else
		messages::add(gdata, "You don't have any healing surges!", COLOR_NEGATIVE);
}


//	Throw selected item at a point.
void triggerThrownAttack(gamedataPtr gdata, creaturePtr thrower, itemPtr thrown, intpair at)
{
	//	if we throw a ranged weapon too far, it may not hit exactly the point we aimed at
	auto spt = thrower->_pos;
	auto dist = hypot(spt.first - at.first, spt.second - at.second);
	bool is_accurate = dist <= 8;

	//	if the shot is inaccurate, modify the target point
	if (!is_accurate)
	{
		at.first += randint(-1, 1);
		at.second += randint(-1, 1);
	}

	//	path to the given point and burst
	bool hit_something = false;
	auto pts = getBresenhamLine(spt, at);
	for (auto pt : pts)
	{
		if (!gdata->_map->isWalkable(pt))
		{
			hitPointWithThrownWeapon(gdata, thrown, pt);
			hit_something = true;
			break;
		}
	}
	if (!hit_something)
		hitPointWithThrownWeapon(gdata, thrown, at);
}


//	Fire an arrow at a target point
void playerTriggerRangedAttack(gamedataPtr gdata, const intpair at)
{
	if (gdata->_player->_ammo != nullptr && gdata->_player->_ammo->_amount > 0)
	{
		//	don't shoot urself jeez
		if (at == gdata->_player->_pos)
			messages::add(gdata, "Please don't shoot yourself!", COLOR_WHITE);

		{
			//	fire the arrow
			gdata->_player->_ammo->_amount -= 1;
			addAnimation(gdata, anim_BulletPath(getBresenhamLine(gdata->_player->_pos, at), gdata->_player->_ammo->getColor()));

			//	did we hit anything?
			auto t = gdata->_map->getCreature(at);
			if (t != nullptr)
				triggerWeaponAttack(gdata, gdata->_player, t, false);

			//	didn't shoot at anything. drop the arrow
			else
			{
				auto drop = itemPtr(new item(*gdata->_player->_ammo));
				drop->_amount = 1;
				gdata->_map->addItemNear(drop, at);
			}
		}
	}
	else
		messages::add(gdata, "You don't have any ammo!", COLOR_NEGATIVE);
	gdata->_state = STATE_NORMAL;
}

//	Check for special attacks/etc
void playerTriggerMeleeAttack(gamedataPtr gdata, creaturePtr target)
{
	//	can we cleave?
	if (gdata->_player->hasMeleeWeaponTag(WTAG_CLEAVE))
		triggerCleaveAttack(gdata, gdata->_player);

	//	normal attack
	else
		triggerWeaponAttack(gdata, gdata->_player, target, true, false, 0);
}


//	Start aiming our ranged weapon.
void playerAimRanged(gamedataPtr gdata)
{
	if (gdata->_player->_ranged != nullptr)
	{
		gdata->_state = STATE_AIM_RANGED;
		gdata->_cursorPt = autotargetCursor(gdata);
		messages::add(gdata, "Select a target...", COLOR_WHITE);
	}
	else
		messages::add(gdata, "You don't have a ranged weapon equipped!", COLOR_WARNING);
}


//	Start aiming a thrown weapon, if a valid one is given.
void playerAimThrown(gamedataPtr gdata, const int idx)
{
	if (idx >= 0 && idx < gdata->_carriedBombs.size())
	{
		gdata->_throwing = gdata->_carriedBombs[idx];
		gdata->_state = STATE_AIM_THROWN;
		gdata->_cursorPt = autotargetCursor(gdata);
		messages::add(gdata, "Select a target for your " + gdata->_throwing->getName() + "...", COLOR_WHITE);
	}
}
