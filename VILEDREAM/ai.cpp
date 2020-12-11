#include "ai.h"


//	Move to the designated point.
void ai::executeMove(gamedataPtr gdata, monsterPtr ai, intpair to)
{
	//	set our position & our action delay
	ai->_pos = to;
	enterTile(gdata, ai);
	ai->setActionDelayTo(ai->getMoveActionDelay());

	//	special
	if (ai->hasAffliction(AFFL_AGONY))
		inflictAgonyDamage(gdata, ai);
}


//	Returns True if we move successfully.
bool ai::moveToPoint(gamedataPtr gdata, monsterPtr ai, intpair toPt)
{
	auto bestPt = getMoveNearerToPoint(gdata, ai, ai->_pos, toPt);
	if (bestPt != ai->_pos)
	{
		ai::executeMove(gdata, ai, bestPt);
		return true;
	}
	return false;
}


//	Returns True if we move successfully.
bool ai::moveAwayFromPoint(gamedataPtr gdata, monsterPtr ai, intpair awayFrom)
{
	auto bestPt = getMoveFurtherFromPoint(gdata, ai, ai->_pos, awayFrom);
	if (bestPt != ai->_pos)
	{
		ai::executeMove(gdata, ai, bestPt);
		return true;
	}
	return false;
}


void ai::doCombatAction(gamedataPtr gdata, monsterPtr ai)
{
	//	If we can see our target, we mark their position.
	if (canMonsterSeeCreature(gdata, ai, ai->_target))
		ai->_targetPt = ai->_target->_pos;


	//	Are we going to do a random move for some reason?
	if ((ai->hasAffliction(AFFL_STAGGER) && roll_one_in(2)) || (ai->hasFlag("flits") && roll_one_in(3)))
	{
		moveToPoint(gdata, ai, getValidConfusionMove(gdata, ai, ai->_pos));
		return;
	}


	//	Are we puking?
	else if (ai->hasAffliction(AFFL_NAUSEA) && roll_one_in(2))
	{
		messages::ai_puke(gdata, ai);
		ai->setActionDelay();
		return;
	}


	//	Do we want to use a special ability?
	if (tryUseSpecialAbility(gdata, ai))
		return;


	//	Can we melee our target?
	if (!ai->hasFlag("no_melee"))
	{
		auto melee_ran = 1.5;
		if (ai->hasMeleeWeaponTag(WTAG_REACH))
			melee_ran = 2.1;
		if (getDistanceBetweenCreatures(ai, ai->_target) < melee_ran)
		{
			if (!tryTouchAttack(gdata, ai, ai->_target))
				triggerWeaponAttack(gdata, ai, ai->_target, true);
			return;
		}
	}


	//	Can we move?
	if (!ai->hasFlag("immobile"))
	{
		//	Cowards may attempt to flee instead of attacking.
		if (ai->hasFlag("cowardly") && roll_one_in(3))
			moveAwayFromPoint(gdata, ai, ai->_targetPt);

		//	If not, move towards the last target point we set.
		else
			moveToPoint(gdata, ai, ai->_targetPt);
	}
}


//	Returns true if we can target the given creature (we have to be able to see them)
bool ai::canTargetCreature(gamedataPtr gdata, monsterPtr ai, creaturePtr t)
{
	return hypot(ai->_pos.first - t->_pos.first, ai->_pos.second - t->_pos.second) <= AI_MAX_DETECTION_RADIUS;
}


//	Checks whether we want to continue targeting our current target; clears target if so.
void ai::checkTargetValidity(gamedataPtr gdata, monsterPtr ai)
{
	if (ai->_target != nullptr)
	{
		if ((ai->_target->isDead()))
		{
			ai->_target = nullptr;
			ai->_targetPt.first = -1;
		}
	}
}


//	AI will now try to kill this creature.
void ai::setTarget(gamedataPtr gdata, monsterPtr ai, creaturePtr t)
{
	ai->_target = t;
	ai->_targetPt = t->_pos;
}



//	Looks for a target that is NOT the player.
void ai::findMonsterTarget(gamedataPtr gdata, monsterPtr ai)
{
	for (auto cr : gdata->_map->getAllCreatures())
	{
		if (ai::canTargetCreature(gdata, ai, cr))
		{
			setTarget(gdata, ai, cr);
			return;
		}
	}
}


//	Look for something we would like to kill.
void ai::findTarget(gamedataPtr gdata, monsterPtr ai)
{
	//	if we can see the player, we target the player.
	if (canMonsterSeeCreature(gdata, ai, gdata->_player))
	{
		setTarget(gdata, ai, gdata->_player);
	}
}


//	Returns True if our turn was expended bc we used a special ability.
//	This checks to see what abilities we HAVE, & decides if we want to use them.
bool ai::tryUseSpecialAbility(gamedataPtr gdata, monsterPtr ai)
{
	//	We can teleport at random.
	if (ai->hasFlag("teleports") && roll_one_in(10))
	{
		auto pt = getTeleportPoint(gdata, ai->_pos, 2, 6);
		addAnimation(gdata, anim_FlashGlyph(ai->_pos, '*', TCODColor::purple));
		executeMove(gdata, ai, pt);
		ai->setActionDelay();
		return true;
	}

	//	We have a pull attack.
	if (ai->hasFlag("pull_attack") && getDistanceBetweenCreatures(ai, ai->_target) > 3 && roll_one_in(2))
	{
		messages::add(gdata, ai->getName() + " drags you!", COLOR_WARNING);
		pull(gdata, ai->_target, ai->_pos, randint(1, 2));
		addAnimation(gdata, anim_BulletPath(getBresenhamLine(ai->_pos, ai->_target->_pos), ai->getColor()));
		ai->setActionDelay();
		return true;
	}

	//	We can spawn something.
	if (!ai->spawns().empty() && roll_one_in(4))
	{
		auto pts = getAdjacentFreePoints(gdata, ai, ai->_pos);
		if (!pts.empty())
		{
			queueMonsterSpawn(gdata, ai->spawns(), pts[randrange(pts.size())]);
			ai->setActionDelay();
			return true;
		}
	}

	return false;
}


//	Checks to see if we have a touch attack we want to use.
//	Returns True if we tried one, false otherwise.
//	Assumes we are already in range for a touch attack.
bool ai::tryTouchAttack(gamedataPtr gdata, monsterPtr ai, creaturePtr t)
{
	if (ai->hasFlag("touch_attack_agony") && !t->hasAffliction(AFFL_AGONY) && roll_one_in(2))
	{
		triggerTouchAttack(gdata, ai, t, AFFL_AGONY, 1 + dieRoll(1, ai->_dangerLevel + 1));
		return true;
	}
	return false;
}


//	AI turns loop through here.
void ai::takeTurn(gamedataPtr gdata, monsterPtr ai)
{
	//	Do we need a target?
	if (!ai->hasTarget())
		findTarget(gdata, ai);


	//	Did we get a target?
	if (ai->hasTarget())
		ai::doCombatAction(gdata, ai);


	//	If we didn't end up doing anything, just delay our turn
	if (ai->isActionDelayClear())
		ai->setActionDelay();
}
