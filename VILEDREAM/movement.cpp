#include "movement.h"




//	Called each turn & applies effects based on the tile we're standing on.
void standOnTile(gamedataPtr gdata, creaturePtr cr)
{
	//	Flying creatures are immune to most surfaces
	if (!cr->isFlying())
	{
		//	Surfaces override tiles effects
		auto sf = gdata->_map->getSurface(cr->_pos);
		if (sf != SURFACE__NONE)
		{
			switch (sf)
			{
			case(SURFACE_BURNING_OIL):	inflictEnergyDamage(gdata, cr, DTYPE_FIRE, randint(4, 4)); break;
			case(SURFACE_ELECTRIC):		inflictEnergyDamage(gdata, cr, DTYPE_ELECTRIC, randint(1, 6)); break;
			case(SURFACE_FIRE):			inflictEnergyDamage(gdata, cr, DTYPE_FIRE, randint(1, 10)); break;
			case(SURFACE_FROST):		cr->addCondition(COND_CHILLED, 10); break;
			case(SURFACE_OIL):			cr->addCondition(COND_OILY, 10); break;

			case(SURFACE_WATER_ELEC):	inflictEnergyDamage(gdata, cr, DTYPE_ELECTRIC, randint(4, 4));
			case(SURFACE_WATER):		cr->addCondition(COND_SOAKED, 10); break;
			}
		}

		//	If there's no surface, check for a tile effect
		else
		{
			switch (gdata->_map->getTile(cr->_pos))
			{
			case(TILE_SAND):
				cr->addCondition(COND_HEATED, 3);
				break;

			case(TILE_WATER_DEEP):
				cr->addCondition(COND_SOAKED, 10);
				inflictDrowning(gdata, cr);
				break;

			case(TILE_WATER_PUTRID):
				tryAddAffliction(gdata, cr, AFFL_NAUSEA, 10);
				break;
			}
		}
	}
}


//	Called when we first step onto a tile, but not on subsequent turns if we remain there.
void enterTile(gamedataPtr gdata, creaturePtr cr)
{
	standOnTile(gdata, cr);
}


//	Push victim away from a target point.
void knockback(gamedataPtr gdata, creaturePtr target, intpair awayFrom, int dist)
{
	auto pt = target->_pos;
	auto vec = get2dVector(awayFrom, pt);
	while (dist-- > 0)
	{
		intpair_add(&pt, &vec);
		if (gdata->_map->isPointClear(pt))
		{
			target->_pos = pt;
			if (target->isPlayer())
				setPlayerPosition(gdata, pt);
			standOnTile(gdata, target);
		}
		else
			break;
	}
}


//	Drags target nearer a given point.
void pull(gamedataPtr gdata, creaturePtr target, intpair to, int dist)
{
	auto pt = target->_pos;
	auto vec = get2dVector(pt, to);
	while (dist-- > 0)
	{
		intpair_add(&pt, &vec);
		if (gdata->_map->isPointClear(pt))
		{
			target->_pos = pt;
			if (target->isPlayer())
				setPlayerPosition(gdata, pt);
			standOnTile(gdata, target);
		}
		else
			break;
	}
}



//	Player tries to walk into a non-walkable tile.
void playerBumpTile(gamedataPtr gdata, const intpair pt)
{
	auto tl = gdata->_map->getTile(pt);

	//	open equipment menu
	if (tl == TILE_STASH_EQUIPMENT)
		openEquipmentMenu(gdata, true);

	//	shop
	else if (gdata->_map->hasItemShop(pt))
	{
		gdata->_currentShop = gdata->_map->getItemShop(pt);
		gdata->_state = STATE_VIEW_SHOP;
	}

	//	can't move here
	else
		messages::add(gdata, "Blocked by a " + getMaptileName(tl) + "!", COLOR_HIGHLIGHT);
}


//	Updates the player's position on the map (moving the FOV, etc)
void setPlayerPosition(gamedataPtr gdata, intpair pt)
{
	gdata->_player->_pos = pt;
	gdata->_map->setFOVMapCentre(pt);
}


//	Happens when we step onto a tile (but does not recur if we linger there)
void playerEnterTile(gamedataPtr gdata, intpair pt)
{
	//	Check for stuff to pick up
	tryAutopickup(gdata, pt);

	//	Observe if there's an item here
	auto it = gdata->_map->getItem(pt);
	if (it != nullptr)
		messages::add(gdata, "You see " + it->getVerboseName() + ".", it->getColor());

	//	And/or stairs
	auto st = gdata->_map->getStairs(pt);
	if (st != nullptr)
		messages::add(gdata, "You see here " + st->_name + "!", COLOR_WHITE);

	//	Set our position
	setPlayerPosition(gdata, pt);
	enterTile(gdata, gdata->_player);
}


//	Attempts to move the player along the given vector.
//	Returns True if we actually moved.
bool doPlayerMove(gamedataPtr gdata, const intpair vec, bool allowAttacking)
{
	//	Get the point we'll be moving to.
	intpair pt = gdata->_player->_pos;
	intpair_add(&pt, &vec);


	//	If we're nauseated, the action might fail
	if (gdata->_player->hasAffliction(AFFL_NAUSEA) && roll_one_in(2))
	{
		messages::add(gdata, "You vomit profously!", COLOR_NEGATIVE);
		gdata->_player->setActionDelay();
		return false;
	}


	if (gdata->_map->inBounds(pt))
	{
		//	Is there a creature here?
		auto cr = gdata->_map->getCreature(pt);

		//	Melee attack
		if (cr != nullptr)
		{
			playerTriggerMeleeAttack(gdata, cr);
			return false;
		}

		//	Is the point walkable?
		else if (gdata->_map->isWalkable(pt))
		{
			//	Are we immobilized?
			if (!gdata->_player->canMove())
			{
				messages::add(gdata, "You can't move!", COLOR_NEGATIVE);
				return false;
			}

			//	complete the move
			else
			{
				//	movement
				playerEnterTile(gdata, pt);
				if (gdata->_player->hasAffliction(AFFL_AGONY))
					inflictAgonyDamage(gdata, gdata->_player);

				//	check for special attack
				if (gdata->_player->hasMeleeWeaponTag(WTAG_LUNGE))
					checkForLungeAttack(gdata, gdata->_player, vec);
				gdata->_player->setActionDelayTo(gdata->_player->getMoveActionDelay());
				gdata->_player->_movedLastTurn = true;
				return true;
			}
		}

		//	Can't move here!
		else
			playerBumpTile(gdata, pt);
	}

	return false;
}

void startAutoMoving(gamedataPtr gdata, const intpair vec)
{
	gdata->_automoveVec = vec;
	gdata->_automoveDist = 3;
}


void doAutomove(gamedataPtr gdata)
{
	//	Try to move.
	if (doPlayerMove(gdata, gdata->_automoveVec, false))
	{
		gdata->_automoveDist--;
	}

	//	If we fail to move, cancel the automove.
	else
	{
		gdata->_automoveDist = 0;
	}
}
