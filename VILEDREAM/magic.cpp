#include "magic.h"


//	Pick up a spellbook. This automatically teaches us some spell knowledge.
void playerGetSpellbook(gamedataPtr gdata, itemPtr it)
{
	int casts = dieRoll(10 - getSpellCost(it->_containsSpell), 6);
	gdata->_player->addSpellKnowledge(it->_containsSpell, casts);
	messages::add(gdata, "You have gained spell knowledge!", COLOR_HIGHLIGHT_POS);
}


void openSpellList(gamedataPtr gdata)
{
	if (!gdata->_player->_spellsKnown.empty())
	{
		gdata->_state = STATE_VIEW_SPELLS;
		gdata->_idx = -1;
	}
	else
		messages::add(gdata, "You don't know any spells!", COLOR_WHITE);
}


//	Open the spell menu so we can pick a spell to cast.
void openSpellSelectMenu(gamedataPtr gdata)
{
	if (!gdata->_player->_spellsKnown.empty())
	{
		gdata->_state = STATE_SELECT_SPELL_TO_CAST;
		gdata->_idx = -1;
	}
	else
		messages::add(gdata, "You don't know any spells!", COLOR_WHITE);
}


//	Attempt to cast the spell at the given index.
void selectSpellToCast(gamedataPtr gdata, const int idx)
{
	if (idx >= 0 && idx < gdata->_player->_spellsKnown.size())
	{
		//	do we have enough MP?
		auto sp = gdata->_player->_spellsKnown[idx];
		if (gdata->_player->hasMagicPointsLeft(getSpellCost(sp)))
		{
			//	do we have castings left?
			if (gdata->_player->knowsSpell(sp))
			{
				//	trigger the spell
				if (!isTargetedSpell(sp))
				{
					expendSpellCost(gdata, sp);
					triggerInstantSpell(gdata, sp);
					gdata->_state = STATE_NORMAL;
				}

				//	we need to aim the spell
				else
				{
					gdata->_casting = sp;
					gdata->_state = STATE_AIM_SPELL;
					messages::add(gdata, "Select a direction to cast:", COLOR_WHITE);
				}
			}
			else
				messages::add(gdata, "Out of spell castings!", COLOR_HIGHLIGHT_NEG);
		}
		else
			messages::add(gdata, "Not enough magic points!", COLOR_HIGHLIGHT_NEG);
	}
}


//	For spells that can be evaded. Returns True if the spell hit.
bool rollSpellAttack(gamedataPtr gdata, creaturePtr target)
{
	int acc = gdata->_player->getBaseAccuracy() + gdata->_player->getAttributeModifier(ATTR_CHARISMA);
	return (dieRoll(2, 10) + acc >= target->getEvasion());
}


//	Pay the cost of a spell: MP, castings, and time.
void expendSpellCost(gamedataPtr gdata, const Spell sp)
{
	gdata->_player->loseMagicPoints(getSpellCost(sp));
	gdata->_player->spendSpellKnowledge(sp);
	gdata->_player->setActionDelay();
}


//	Effect of an instant spell.
void triggerInstantSpell(gamedataPtr gdata, const Spell sp)
{
	messages::add(gdata, "You cast " + getSpellName(sp) + "!", getSpellColor(sp));

	switch (sp)
	{
	case(SPELL_EXCRUCIATION):
		spell_AllVisible(gdata, sp);
		break;

	case(SPELL_FIREBALL):		spell_Burst(gdata, sp, 3);	break;

	case(SPELL_ELECTRIFY):		spell_Electrify(gdata); break;

	case(SPELL_ARCANE_EMANATION):
	case(SPELL_CALL_LIGHTNING):
		spell_RandomTarget(gdata, sp);
		break;
	}
}


//	Fire a spell in a given direction.
void triggerSpellInDirection(gamedataPtr gdata, const Spell sp, const intpair vec)
{
	switch (sp)
	{
		//	Spell strikes every target in a line
	case(SPELL_BLOOD_SPEW):
		creatureTakeDamage(gdata, gdata->_player, dieRoll(2, 6));
	case(SPELL_CRYSTAL_SPEAR):
	case(SPELL_LIGHTNING_SPEAR):
		spell_Penetrating(gdata, sp, vec);
		break;

		//	Spell travels in a straight line and hits a point
	case(SPELL_FRIGID_BLAST):
	case(SPELL_ICE_SPIKE):
	case(SPELL_SLAY_LIVING):
	case(SPELL_THROW_FIRE):
		spell_Ray(gdata, sp, vec);
		break;
	}

	expendSpellCost(gdata, sp);
	gdata->_state = STATE_NORMAL;
}



//	Affects every visible enemy.
void spell_AllVisible(gamedataPtr gdata, const Spell sp)
{
	for (auto t : gdata->_visibleMonsters)
	{
		switch (sp)
		{
		case(SPELL_EXCRUCIATION):
			addAnimation(gdata, anim_FlashGlyph(t->_pos, '!', getSpellColor(sp)));
			tryAddAffliction(gdata, t, AFFL_AGONY, dieRoll(2, 6));
			break;
		}
	}
}



//	Affects everything in an AOE near us.
void spell_Burst(gamedataPtr gdata, const Spell sp, const int rad)
{
	//	the aoe
	auto ctr = gdata->_player->_pos;
	for (int x = ctr.first - rad; x <= ctr.first + rad; x++)
	{
		for (int y = ctr.second - rad; y <= ctr.second + rad; y++)
		{
			if (gdata->_map->inBounds(x, y))
			{
				auto t = gdata->_map->getCreature(x, y);
				if (t != nullptr)
				{
					switch (sp)
					{
					case(SPELL_FIREBALL):
						inflictEnergyDamage(gdata, t, DTYPE_FIRE, dieRoll(3, 6));
						break;
					}
				}
			}
		}
	}

	//	animate
	addAnimation(gdata, anim_Explosion(ctr, rad, '%', getSpellColor(sp)));
}


//	Spell keeps going until it hits an obstacle, affecting every creature it passes through.
void spell_Penetrating(gamedataPtr gdata, const Spell sp, const intpair vec)
{
	//	find the destination point
	auto pt = gdata->_player->_pos;
	intpair_add(&pt, &vec);
	vector<intpair> pts = { pt };
	while (gdata->_map->isWalkable(pt) && gdata->_map->isTransparent(pt))
	{
		//	is there a target here to affect?
		auto t = gdata->_map->getCreature(pt);
		int dam = 0;
		if (t != nullptr)
		{
			switch (sp)
			{
			case(SPELL_BLOOD_SPEW):
				dam = dieRoll(8, 6);
				dam = applyDamageResist(gdata, t, dam);
				creatureTakeDamage(gdata, t, dam);
				break;

			case(SPELL_CRYSTAL_SPEAR):
				inflictEnergyDamage(gdata, t, DTYPE_ETHEREAL, dieRoll(2, 12));
				break;

			case(SPELL_LIGHTNING_SPEAR):
				if (rollSpellAttack(gdata, t))
					inflictEnergyDamage(gdata, t, DTYPE_ELECTRIC, dieRoll(3, 8));
				else
					messages::add(gdata, t->getName() + " dodges!", COLOR_MEDIUM);
				break;
			}
		}

		//	maximum range
		if (pts.size() > getSpellRange(sp))
			break;

		//	next point
		intpair_add(&pt, &vec);
		pts.push_back(pt);
	}
	
	//	animation
	addAnimation(gdata, anim_BulletPath(pts, getSpellColor(sp)));
}


//	Spell that targets a random visible enemy.
void spell_RandomTarget(gamedataPtr gdata, const Spell sp)
{
	if (!gdata->_visibleMonsters.empty())
	{
		//	pick a target & apply the effect
		auto t = gdata->_visibleMonsters[randrange(gdata->_visibleMonsters.size())];
		switch (sp)
		{
		case(SPELL_ARCANE_EMANATION):
			inflictEnergyDamage(gdata, t, DTYPE_ETHEREAL, randint(2, 4) + 2);
			break;

		case(SPELL_CALL_LIGHTNING):
			inflictEnergyDamage(gdata, t, DTYPE_ELECTRIC, randint(1, 12));
			break;
		}

		//	animation
		auto pts = getBresenhamLine(gdata->_player->_pos, t->_pos);
		addAnimation(gdata, anim_BulletPath(pts, getSpellColor(sp)));
	}
	else
		messages::add(gdata, "The spell couldn't find a target!", COLOR_NEGATIVE);
}


//	Spell travels in a straight line until it hits something, then triggers its effect.
void spell_Ray(gamedataPtr gdata, const Spell sp, const intpair vec)
{
	//	find the destination point
	auto pt = gdata->_player->_pos;
	intpair_add(&pt, &vec);
	vector<intpair> pts = { pt };
	while (gdata->_map->getCreature(pt) == nullptr && gdata->_map->isWalkable(pt) && gdata->_map->isTransparent(pt))
	{
		//	next point
		intpair_add(&pt, &vec);
		pts.push_back(pt);

		//	maximum range
		if (pts.size() >= getSpellRange(sp))
			break;
	}
	auto target = gdata->_map->getCreature(pt);

	//	special animation
	addAnimation(gdata, anim_BulletPath(pts, getSpellColor(sp)));

	//	have our effect there
	switch (sp)
	{
		//	Staggering ice.
	case(SPELL_ICE_SPIKE):
		if (target != nullptr)
		{
			inflictEnergyDamage(gdata, target, DTYPE_COLD, randint(1, 8));
			tryAddAffliction(gdata, target, AFFL_STAGGER, randint(3, 5));
		}
		break;


		//	Ice with knockback power.
	case(SPELL_FRIGID_BLAST):
		for (int x = pt.first - 1; x <= pt.first + 1; x++)
		{
			for (int y = pt.second - 1; y <= pt.second + 1; y++)
			{
				if (gdata->_map->inBounds(x, y))
				{
					tryAddSurface(gdata, SURFACE_FROST, intpair(x, y));
					auto t = gdata->_map->getCreature(x, y);
					if (t != nullptr && !t->isPlayer())
					{
						knockback(gdata, target, gdata->_player->_pos, randint(2, 4));
						inflictEnergyDamage(gdata, t, DTYPE_COLD, dieRoll(3, 6));
					}
				}
			}
		}
		break;


		//	Massive necrotic damage.
	case(SPELL_SLAY_LIVING):
		if (target != nullptr)
		{
			if (rollSpellAttack(gdata, target))
				inflictEnergyDamage(gdata, target, DTYPE_NECROTIC, dieRoll(10, 6));
			else
				messages::add(gdata, "You missed!", COLOR_HIGHLIGHT_NEG);
		}
		break;


		//	Fire AOE.
	case(SPELL_THROW_FIRE):
		for (int x = pt.first - 1; x <= pt.first + 1; x++)
		{
			for (int y = pt.second - 1; y <= pt.second + 1; y++)
			{
				if (gdata->_map->inBounds(x, y))
				{
					tryAddSurface(gdata, SURFACE_FIRE, intpair(x, y));
					auto t = gdata->_map->getCreature(x, y);
					if (t != nullptr)
						inflictEnergyDamage(gdata, t, DTYPE_FIRE, randint(1, 6));
				}
			}
		}
		break;
	}
}


void spell_Electrify(gamedataPtr gdata)
{
	auto ctr = gdata->_player->_pos;
	int r = 10;
	for (int x = ctr.first - r; x <= ctr.first + r; x++)
	{
		for (int y = ctr.second - r; y <= ctr.second + r; y++)
		{
			if (gdata->_map->inBounds(x, y) && !(x == ctr.first && y == ctr.second))
				tryAddSurface(gdata, SURFACE_ELECTRIC, intpair(x, y));
		}
	}
	addAnimation(gdata, anim_FlashGlyph(ctr, '*', getSpellColor(SPELL_ELECTRIFY)));
}
