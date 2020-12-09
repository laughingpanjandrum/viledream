#include "display.h"



display::display()
{
	//	Load RXP images into memory
	loadImages();

	//	Initialize empty arrays of visible map tiles
	for (unsigned int x = 0; x < MAP_X_SIZE; x++)
	{
		vector<int> xglyphs;
		vector<colorType> xcols;
		vector<colorType> xbgcols;
		for (unsigned int y = 0; y < MAP_Y_SIZE; y++)
		{
			xglyphs.push_back(0);
			xcols.push_back(COLOR_BLACK);
			xbgcols.push_back(COLOR_BLACK);
		}
		_visibleGlyphs.push_back(xglyphs);
		_visibleColors.push_back(xcols);
		_visibleBgcolors.push_back(xbgcols);
	}

	//	percentile colour map
	int idx[] = { 0, 50, 100 };
	TCODColor col[] = { TCODColor::red, TCODColor::yellow, TCODColor::green };
	TCODColor::genMap(_percentColors, 3, col, idx);
}


void display::drawAttackByline(int x, int y, creaturePtr cr, itemPtr wp, bool is_ranged)
{
	string txt;
	if (wp == nullptr)	txt += "unarmed";
	else				txt += wp->getName();

	//	get attack attributes
	auto acc = (is_ranged) ? cr->getRangedAccuracy() : cr->getMeleeAccuracy();
	auto die = (is_ranged) ? cr->getRangedDamageDice() : cr->getMeleeDamageDice();
	auto bns = (is_ranged) ? cr->getRangedDamageBonus() : cr->getMeleeDamageBonus();
	

	//	format the attributes
	txt += " " + plusminus(acc);
	txt += " (" + to_string(die.first) + 'd' + to_string(die.second);
	if (bns != 0)	txt += plusminus(bns);

	txt += ")";
	auto col = COLOR_WHITE;
	if (wp != nullptr)
		col = wp->getColor();
	
	_win.write(x, y, txt, col);
}


void display::drawCharacterSheet(gamedataPtr gdata)
{
	drawRXPImage(_img_Main, 0, 0);
	auto p = gdata->_player;


	//	vital statistics
	_win.write(58, 4, "VITAL STATISTICS", COLOR_LIGHT);
	for (unsigned i = 0; i < ATTR__MAX; i++)
	{
		auto attr = static_cast<Attribute>(i);
		int y = 6 + i * 2;

		drawDottedLinePair(59, y, 79, getAttributeName(attr), to_string(p->getDerivedAttributeValue(attr)));
		_win.write(83, y, "[" + plusminus(p->getAttributeModifier(attr)) + "]", COLOR_MEDIUM);
	}


	//	secondary attributes
	_win.write(58, 18, "DERIVED STATISTICS", COLOR_LIGHT);

	drawDottedLinePair(59, 20, 79, "Base Accuracy", "[" + plusminus(p->getBaseAccuracy()) + "]");
	drawDottedLinePair(59, 22, 79, "Base Dmg Bns", "[" + plusminus(p->getBaseDamageBonus()) + "]");

	
	//	defences
	_win.write(58, 25, "DEFENCES", COLOR_LIGHT);

	drawDottedLinePair(59, 27, 79, "Evasion", to_string(p->getEvasion()));
	drawDottedLinePair(59, 29, 79, "Block", to_string(p->getBlockValue()));
	drawDottedLinePair(59, 31, 79, "Damage Resist", to_string(p->getDamageResist()));



	//	left-hand stuff

	writeFormatted(4, 4, "Health    #" + to_string(p->getHealthLeft()) + "@/" + to_string(p->getMaxHealth()), { TCODColor::green });
	_win.write(4, 5, "Magic Pts", COLOR_MEDIUM);
	for (unsigned i = 0; i < p->getMaxMagicPoints(); i++)
		_win.writec(14 + i, 5, 9, TCODColor::lightBlue);


	//	attacks

	_win.write(4, 8, "ATTACKS", COLOR_LIGHT);

	drawAttackByline(5, 10, p, p->_weapon, false);
	writeFormatted(6, 11, "critical #" + to_string(p->getMeleeCritChance()) + "% @(#x" + to_string(p->getMeleeCritMultiplier()) + "@)", 
		{ COLOR_LIGHT, COLOR_LIGHT });
	writeFormatted(6, 12, "no. of attacks #" + to_string(p->getMeleeAttackCount()), { COLOR_LIGHT });


	if (p->_ranged != nullptr)
	{
		drawAttackByline(5, 14, p, p->_ranged, true);
	}


	//	resistances
	_win.write(4, 17, "RESISTANCES", COLOR_MEDIUM);
	
	int y = 19;
	for (auto res : RESISTANCES_PRIMARY)
	{
		auto val = p->getResistance(res);
		drawDottedLinePair(5, y, 16, getResistanceName(res), to_string(val) + "%", getResistantValueColor(val), getResistanceColor(res));
		y++;
	}
	y += 1;
	for (auto res : RESISTANCES_SECONDARY)
	{
		auto val = p->getResistance(res);
		drawDottedLinePair(5, y, 16, getResistanceName(res), to_string(val) + "%", getResistantValueColor(val), getResistanceColor(res));
		y++;
	}
}


//	Lists our known spells.
void display::drawSpellList(gamedataPtr gdata)
{
	drawRXPImage(_img_Main, 0, 0);
	drawSidebar(gdata);


	//	Title
	_win.write(4, 4, "SPELLS KNOWN", COLOR_LIGHT);
	_win.write(35, 4, "MP", COLOR_MEDIUM);
	_win.write(40, 4, "Casts", COLOR_MEDIUM);


	//	List of known spells
	for (unsigned i = 0; i < gdata->_player->_spellsKnown.size(); i++)
	{
		int y = 6 + i * 2;

		//	selection
		if (i == gdata->_idx)
			_win.writec(5, y, CHAR_SET_LOWERCASE[i], COLOR_HIGHLIGHT);
		else
			_win.writec(5, y, CHAR_SET_LOWERCASE[i], COLOR_MEDIUM);

		//	the spell
		auto sp = gdata->_player->_spellsKnown[i];
		drawDottedLinePair(7, y, 35, getSpellName(sp), to_string(getSpellCost(sp)), TCODColor::lightBlue, getSpellColor(sp));
		_win.write(40, y, to_string(gdata->_player->_spellCastsLeft[i]), COLOR_LIGHT);
	}


	//	Description of selected spell, if any
	if (gdata->_idx >= 0 && gdata->_idx < gdata->_player->_spellsKnown.size())
	{
		auto sp = gdata->_player->_spellsKnown[gdata->_idx];
		_win.write(4, 48, getSpellName(sp), getSpellColor(sp));

		_win.write(4, 49, "MP Cost:", COLOR_MEDIUM);
		for (unsigned i = 0; i < getSpellCost(sp); i++)
			_win.writec(13 + i * 2, 49, 9, TCODColor::lightBlue);

		writeFormatted(4, 50, "Castings Left: #" + to_string(gdata->_player->_spellCastsLeft[gdata->_idx]), { COLOR_LIGHT });

		_win.writeWrapped(4, 52, 45, getSpellDescription(sp), COLOR_MEDIUM);
	}

	//	If no spell is selected, draw messages in that box.
	else
		drawMessages(gdata);
}


//	What items we have equipped
void display::drawEquipment(gamedataPtr gdata)
{
	drawRXPImage(_img_Main, 0, 0);
	drawSidebar(gdata);

	_win.write(4, 4, "EQUIPPED ITEMS", COLOR_MEDIUM);
	auto p = gdata->_player;
	auto ilist = p->getAllEquippedItems();
	auto catlist = p->getEquippedItemCategories();
	for (unsigned i = 0; i < ilist.size(); i++)
	{
		//	letter corresponding to the slot
		int y = 6 + i * 2;
		_win.writec(5, y, CHAR_SET_LOWERCASE[i], (gdata->_idx == i ? COLOR_WHITE : COLOR_MEDIUM));

		//	item here, if any
		auto it = ilist[i];
		if (it != nullptr)
		{
			_win.writec(7, y, it->getGlyph(), it->getColor());
			_win.write(9, y, it->getVerboseName(), it->getColor());
		}
		else
			_win.write(9, y, "[no " + getItemCategoryName(catlist[i]) + "]", TCODColor::darkGrey);
	}

	//	interaction options
	if (gdata->_state == STATE_MODIFY_EQUIPMENT)
	{
		writeFormatted(4, 40, "[ #a-z @] #Select an equipment slot", { COLOR_LIGHT, COLOR_LIGHT });
		writeFormatted(4, 41, "[#ENTER@] #Select an item to equip", { COLOR_LIGHT, COLOR_LIGHT });
		writeFormatted(4, 42, "[#  R  @] #Un-equip selected item", { COLOR_LIGHT, COLOR_LIGHT });
	}

	//	description of selected item
	if (gdata->_idx >= 0 && gdata->_idx < ilist.size() && ilist[gdata->_idx] != nullptr)
		drawItemInfo(gdata, ilist[gdata->_idx], 4, 48, true);
	else
		drawMessages(gdata);
}


//	Player picks an attribute to improve
void display::drawAttributeSelection(gamedataPtr gdata)
{
	//	surrounding interface
	drawRXPImage(_img_Main, 0, 0);
	drawSidebar(gdata);
	drawMessages(gdata);

	//	options
	_win.write(4, 4, "Select an attribute to raise:", COLOR_LIGHT);
	for (unsigned i = 0; i < ATTR__MAX; i++)
	{
		auto attr = static_cast<Attribute>(i);
		int y = 6 + i * 5;
		_win.writec(4, y, CHAR_SET_LOWERCASE[i], COLOR_MEDIUM);
		
		_win.write(6, y, getAttributeName(attr), COLOR_LIGHT);
		writeFormatted(20, y, "[#" + to_string(gdata->_player->getBaseAttributeValue(attr)) + "@]", { COLOR_LIGHT });
		
		_win.writeWrapped(6, y + 1, 40, getAttributeDescription(attr), COLOR_DARK);
	}
}


//	Shows player's feat slots.
void display::drawFeatsKnown(gamedataPtr gdata)
{
	drawRXPImage(_img_Main, 0, 0);
	drawSidebar(gdata);

	_win.write(4, 4, "FEAT SLOTS", COLOR_MEDIUM);

	for (unsigned i = 0; i < gdata->_player->_featSlots.size(); i++)
	{
		int y = 6 + i * 2;
		_win.writec(5, y, CHAR_SET_LOWERCASE[i], (gdata->_idx == i) ? COLOR_WHITE : COLOR_DARK);

		auto ft = gdata->_player->_featSlots[i];
		if (ft != FEAT__MAX)
			_win.write(7, y, getFeatName(ft), COLOR_LIGHT);
		else if (gdata->_player->isFeatSlotAvailable(i))
			_win.write(7, y, "[Feat Available]", TCODColor::lightGrey);
		else
			_win.write(7, y, "[Locked]", TCODColor::darkGrey);
	}

	//	description of selected feat, if any
	if (gdata->_idx >= 0 && gdata->_idx < gdata->_player->_featSlots.size() && gdata->_player->_featSlots[gdata->_idx] != FEAT__MAX)
	{
		auto ft = gdata->_player->_featSlots[gdata->_idx];
		_win.write(4, 48, getFeatName(ft), COLOR_MEDIUM);
		_win.writeWrapped(5, 50, 45, getFeatDescription(ft), COLOR_LIGHT);
	}
	else
		drawMessages(gdata);
}


//	Lists ALL feats, indicating which ones are already known if necessary.
void display::drawFeatsToBuy(gamedataPtr gdata)
{
	drawRXPImage(_img_Main, 0, 0);
	drawSidebar(gdata);

	_win.write(4, 4, "SELECT A FEAT TO LEARN", COLOR_MEDIUM);

	for (unsigned i = 0; i < ALL_FEATS.size(); i++)
	{
		int y = 6 + i;
		_win.writec(5, y, CHAR_SET_LOWERCASE[i], (gdata->_idx == i) ? COLOR_WHITE : COLOR_DARK);

		auto ft = ALL_FEATS[i];
		if (gdata->_player->knowsFeat(ft))
			_win.write(7, y, '*' + getFeatName(ft), COLOR_DARK);
		else
			_win.write(7, y, getFeatName(ft), COLOR_LIGHT);
	}

	//	description of selected
	if (gdata->_idx >= 0 && gdata->_idx < ALL_FEATS.size())
	{
		auto ft = ALL_FEATS[gdata->_idx];
		_win.write(4, 48, getFeatName(ft), COLOR_MEDIUM);
		if (gdata->_player->knowsFeat(ft))
			_win.write(4, 49, "You know this feat already.", COLOR_WHITE);
		else
			_win.write(4, 49, "Press ENTER to learn this feat.", COLOR_LIGHT);

		_win.writeWrapped(5, 51, 45, getFeatDescription(ft), COLOR_LIGHT);
	}
}


//	Detailed info about a monster.
void display::drawMonsterInfo(gamedataPtr gdata, monsterPtr mon)
{
	drawRXPImage(_img_Main, 0, 0);
	drawSidebar(gdata);
	drawMessages(gdata);
	_win.writec(4, 4, mon->getGlyph(), mon->getColor());
	_win.write(6, 4, mon->getName(), mon->getColor());

	writeFormatted(5, 6, "HP #" + to_string(mon->getMaxHealth()) + " @- EV #" + to_string(mon->getEvasion()) + " @- BL #" + 
		to_string(mon->getBlockValue()) + " @- DR #" + to_string(mon->getDamageResist()),
		{ TCODColor::red, COLOR_LIGHT, COLOR_LIGHT, COLOR_LIGHT });

	//	base melee attack
	auto mdmg = mon->getMeleeDamageDice();
	auto mbns = mon->getMeleeDamageBonus();
	auto dstr = "melee attack: #" + plusminus(mon->getMeleeAccuracy()) + " @(#" + to_string(mdmg.first) + "d" + to_string(mdmg.second);
	vector<colorType> vcols = { COLOR_LIGHT, COLOR_LIGHT };
	if (mbns != 0) dstr += plusminus(mbns);


	//	extra damage?
	for (auto dt : SPECIAL_DAMAGE_TYPES)
	{
		auto ddie = mon->getMeleeSpecialDamage(dt);
		if (ddie.first > 0)
		{
			dstr += "#+" + to_string(ddie.first) + "d" + to_string(ddie.second) + " " + getDamageTypeName(dt);
			vcols.push_back(getDamageTypeColor(dt));
		}
	}

	//	show the attack
	dstr += "@)";
	writeFormatted(5, 8, dstr, vcols);

	int y = 8;
	for (unsigned i = 0; i < AFFL__NONE; i++)
	{
		auto af = static_cast<Affliction>(i);
		if (mon->getMeleeAfflictionStacks(af) > 0)
			writeFormatted(5, ++y, "Inflicts #" + getAfflictionName(af), { getAfflictionColor(af) });
	}



	//	resistances
	y += 2;
	for (unsigned i = 0; i < RESIST__NONE; i++)
	{
		auto res = static_cast<ResistanceType>(i);
		auto rval = mon->getResistance(res);
		if (rval != 0)
		{
			_win.write(10, ++y, getResistanceName(res) + " Resist", getResistanceColor(res));
			_win.write(5, y, to_string(rval) + "%", getResistantValueColor(rval));
		}
	}


	//	hit chances
	y += 2;
	
	_win.write(4, ++y, "vs your melee attacks, it has:", COLOR_MEDIUM);
	auto acc = gdata->_player->getMeleeAccuracy();
	auto ev = mon->getEvasion();
	writeFormatted(5, ++y, "a #" + to_string(100 - getHitPercent_2d10(acc, ev)) + "% @chance to evade", { COLOR_LIGHT });

	auto blk = mon->getBlockValue();
	if (blk > 0)
		writeFormatted(5, ++y, "a #" + to_string(100 - getHitPercent_2d10(acc, blk)) + "% @chance to block", { COLOR_LIGHT });


	++y;
	_win.write(4, ++y, "vs its melee attacks, you have:", COLOR_MEDIUM);
	acc = mon->getMeleeAccuracy();
	ev = gdata->_player->getEvasion();
	writeFormatted(5, ++y, "a #" + to_string(100 - getHitPercent_2d10(acc, ev)) + "% @chance to evade", { COLOR_HIGHLIGHT_POS });

	blk = gdata->_player->getBlockValue();
	if (blk > 0)
		writeFormatted(5, ++y, "a #" + to_string(100 - getHitPercent_2d10(acc, blk)) + "% @chance to block", { COLOR_HIGHLIGHT_POS });


	//	flags
	y += 2;
	
	//		type
	if (mon->hasFlag("construct"))
		_win.write(4, ++y, "It is a construct.", COLOR_MEDIUM);
	if (mon->hasFlag("undead"))
		_win.write(4, ++y, "It is undead.", COLOR_MEDIUM);

	//		special attacks
	if (mon->hasFlag("drowning_attack"))
		_win.write(4, ++y, "It attempts to drown you in melee.", COLOR_MEDIUM);
	if (mon->hasFlag("grapples"))
		_win.write(4, ++y, "It can grapple with melee attacks.", COLOR_MEDIUM);
	if (mon->hasFlag("melee_reach"))
		_win.write(4, ++y, "It can attack at reach.", COLOR_MEDIUM);

	//		special behaviours/abilities
	if (mon->hasFlag("cowardly"))
		_win.write(4, ++y, "It sometimes flees instead of attacking.", COLOR_MEDIUM);
	if (mon->hasFlag("flits"))
		_win.write(4, ++y, "It moves erratically.", COLOR_MEDIUM);
	if (mon->hasFlag("flying"))
		_win.write(4, ++y, "It is flying.", COLOR_MEDIUM);
	if (mon->hasFlag("moves_quickly"))
		_win.write(4, ++y, "It moves quickly.", COLOR_MEDIUM);
	if (mon->hasFlag("moves_slowly"))
		_win.write(4, ++y, "It moves slowly.", COLOR_MEDIUM);
	if (mon->hasFlag("teleports"))
		_win.write(4, ++y, "It sometimes teleports at random.", COLOR_MEDIUM);
}


void display::drawInventory(gamedataPtr gdata, ItemCategory cat, bool show_header)
{
	auto ilist = getItemListForCategory(gdata, cat);
	drawRXPImage(_img_Main, 0, 0);
	_win.write(3, 1, centreText("INVENTORY", 27), COLOR_LIGHT);


	//	Lists inventory categories along the top
	if (show_header)
		drawInventoryHeader(cat);


	//	list of items
	for (unsigned i = 0; i < ilist->size(); i++)
	{
		auto it = ilist->at(i);
		_win.writec(4, 6 + i, CHAR_SET_LOWERCASE[i], (gdata->_idx == i) ? COLOR_WHITE : COLOR_DARK);
	
		_win.writec(6, 6 + i, it->getGlyph(), it->getColor());
		_win.write(8, 6 + i, it->getVerboseName(), it->getColor());
	}

	//	description of selected item
	if (gdata->_idx >= 0 && gdata->_idx < ilist->size())
		drawItemInfo(gdata, ilist->at(gdata->_idx), 4, 49);

	//	other info
	else
		drawMessages(gdata);
	drawSidebar(gdata);
}


//	List inventory categories along the top, with the current one highlighted
void display::drawInventoryHeader(ItemCategory cat)
{
	_win.writec(4, 4, 27, COLOR_LIGHT);
	_win.writec(52, 4, 26, COLOR_LIGHT);
	
	int atx = 10;
	_win.writec(atx, 4, 179, COLOR_DARK);
	atx += 2;

	for (auto tcat : CONSUMABLE_CATEGORIES)
	{
		string name = getItemCategoryName(tcat);
		if (tcat == cat)
			_win.write(atx, 4, name, COLOR_WHITE);
		else
			_win.write(atx, 4, name, COLOR_MEDIUM);

		atx += name.size() + 2;
		_win.writec(atx, 4, 179, COLOR_DARK);
		atx += 2;
	}
}


//	Decide whether to equip or stash an item.
void display::drawEquipmentPickup(gamedataPtr gdata, itemPtr it)
{
	drawRXPImage(_img_Main, 0, 0);
	_win.write(3, 1, "PICKING UP ITEM", COLOR_LIGHT);
	

	//	item we're picking up
	drawItemInfo(gdata, it, 4, 4, false);


	//	what to do with it
	drawLineVertical(29, 6, 11, COLOR_DARK);
	writeFormatted(31, 7, "[#g@] #Send to stash", { COLOR_LIGHT, COLOR_LIGHT });
	writeFormatted(31, 8, "[#e@] #Equip immediately", { COLOR_LIGHT, COLOR_LIGHT });


	//	if it's a weapon, we can have an alt weapon readied as well
	if (it->_category == ITEM_WEAPON || it->_category == ITEM_RING)
		writeFormatted(31, 9, "[#q@] #Equip as alternate", { COLOR_LIGHT, COLOR_LIGHT });


	//	check if we have an equipped item of the same type already
	drawLineHorizontal(3, 18, 51, COLOR_DARK);
	auto eq = gdata->_player->getItemInSlot(it->_category);
	if (eq != nullptr)
	{
		_win.write(4, 20, "You have the following equipped already:", COLOR_LIGHT);
		drawItemInfo(gdata, eq, 5, 22, false);
	}

	//	or show alt item
	auto altit = gdata->_player->getAltItem(it->_category);
	if (altit != nullptr)
	{
		drawLineHorizontal(3, 30, 51, COLOR_DARK);
		_win.write(4, 32, "Alternate item:", COLOR_LIGHT);
		drawItemInfo(gdata, altit, 5, 34, false);
	}


	//	other normal interface stuff
	drawMessages(gdata);
	drawSidebar(gdata);
}

void display::drawItemInfo(gamedataPtr gdata, itemPtr it, int atx, int aty, bool showDescription)
{
	_win.writec(atx, aty, it->getGlyph(), it->getColor());
	_win.write(atx + 2, aty, it->getVerboseName(), it->getColor());
	_win.write(atx + 2, ++aty, getItemCategoryName(it->_category), TCODColor::darkGrey);

	if (showDescription)
	{
		aty += 2;
		aty = _win.writeWrapped(atx, aty, 45, itemData::getDescription(it->_id), COLOR_MEDIUM);
	}

	switch (it->_category)
	{
	case(ITEM_ARMOUR):
		drawArmourInfo(gdata, it, atx, aty + 2); break;

	case(ITEM_CLOAK):
		drawCloakInfo(gdata, it, atx, aty + 2); break;

	case(ITEM_SHIELD):
		drawShieldInfo(gdata, it, atx, aty + 2); break;

	case(ITEM_RANGED):
	case(ITEM_WEAPON):
		drawWeaponInfo(gdata, it, atx, aty + 2); break;
	}
}

void display::drawWeaponInfo(gamedataPtr gdata, itemPtr it, int atx, int aty)
{
	auto dmg = it->getDamageDice();
	auto wtag = it->getWeaponTag();

	drawDottedLinePair(atx, ++aty, atx + 15, "Accuracy", plusminus(it->getAccuracyAdjust()));
	drawDottedLinePair(atx, ++aty, atx + 15, "Damage", to_string(dmg.first) + "d" + to_string(dmg.second));

	if (it->_category == ITEM_RANGED)
		drawDottedLinePair(atx, ++aty, atx + 15, "Range", to_string(it->getAttackRange()));

	auto req = it->getStatRequirement();
	auto scol = COLOR_WHITE;
	if (gdata->_player->getDerivedAttributeValue(ATTR_STRENGTH) < req)
		scol = COLOR_HIGHLIGHT_NEG;
	writeFormatted(atx, ++aty, "Requires #" + to_string(req) + " @Strength", { scol });

	if (wtag != WTAG__NONE)
		_win.write(atx + 1, ++aty, getWeaponTagName(wtag), COLOR_LIGHT);
}

void display::drawArmourInfo(gamedataPtr gdata, itemPtr it, int atx, int aty)
{
	drawDottedLinePair(atx, ++aty, atx + 15, "Evasion", plusminus(it->getEvadeAdjust()));
	drawDottedLinePair(atx, ++aty, atx + 15, "Damage Resist", to_string(it->getDamageResist()));

	auto req = it->getStatRequirement();
	auto scol = COLOR_WHITE;
	if (gdata->_player->getDerivedAttributeValue(ATTR_CONSTITUTION) < req)
		scol = COLOR_HIGHLIGHT_NEG;
	writeFormatted(atx, ++aty, "Requires #" + to_string(req) + " @Constitution", { scol });
}

void display::drawCloakInfo(gamedataPtr gdata, itemPtr it, int atx, int aty)
{
	auto bns = it->getEnchantBonus();
	string txt = "";
	switch (it->getEnchantment())
	{
	case(ENCH_BLADES):
		txt = "Retaliates for #1d" + to_string(bns * 2) + " @damage when you are hit.";
		break;

	case(ENCH_DEFLECTION):
		txt += "It increases your ability to block attacks by #" + plusminus(bns) + "@.";
		break;

	case(ENCH_EVASION):
		txt += "It increases your Evasion by #" + plusminus(bns) + "@.";
		break;

	case(ENCH_RESISTANCE):
		txt += "It increases your resistance to afflictions by #" + plusminus(bns * 5) + "%@.";
		break;
	}

	writeFormatted(atx, ++aty, txt, { COLOR_LIGHT });
}

void display::drawShieldInfo(gamedataPtr gdata, itemPtr it, int atx, int aty)
{
	drawDottedLinePair(atx, ++aty, atx + 15, "Accuracy", plusminus(it->getAccuracyAdjust()));
	drawDottedLinePair(atx, ++aty, atx + 15, "Block Value", to_string(it->getBlockValue()));

	auto req = it->getStatRequirement();
	auto scol = COLOR_WHITE;
	if (gdata->_player->getDerivedAttributeValue(ATTR_DEXTERITY) < req)
		scol = COLOR_HIGHLIGHT_NEG;
	writeFormatted(atx, ++aty, "Requires #" + to_string(req) + " @Dexterity", { scol });
}


//	Attempt to load all images
void display::loadImages()
{
	_img_Main = rexImagePtr(new xp::RexImage("data/img/main.xp"));
}


//			Draw an image at this coordinate.
void display::drawRXPImage(rexImagePtr img, int x, int y)
{
	for (int ix = 0; ix < img->getWidth(); ix++)
	{
		for (int iy = 0; iy < img->getHeight(); iy++)
		{
			xp::RexTile tl = *img->getTile(0, ix, iy);
			auto col = TCODColor(tl.fore_red, tl.fore_green, tl.fore_blue);
			auto bgcol = TCODColor(tl.back_red, tl.back_green, tl.back_blue);
			_win.writec(x + ix, y + iy, tl.character, col, bgcol);
		}
	}
}



//	Re-calculates what is visible on the map.
void display::updateVisibleMapData(gamedataPtr gdata)
{
	//	player character is at the centre of the screen.
	auto ctr = gdata->_player->_pos;
	gdata->_visibleMonsters.clear();


	//	Visible tiles on the map.
	for (int dx = 0; dx < MAP_X_SIZE; dx++)
	{
		for (int dy = 0; dy < MAP_Y_SIZE; dy++)
		{
			//	Default display values
			int gl = 0;
			colorType col = COLOR_BLACK;
			colorType bgcol = COLOR_BLACK;


			//	Is this in the map boundaries
			auto mpt = displayToMapCoord(ctr, dx, dy);
			if (gdata->_map->inBounds(mpt))
			{
				//	Derive into from the base tile.
				auto tl = gdata->_map->getTile(mpt);
				gl = getMaptileGlyph(tl);
				col = getMaptileColor(tl);
				bgcol = COLOR_BLACK;

				//	Is this point in FOV?
				if (gdata->_map->isInFov(mpt) || gdata->_omniscient)
				{
					//	Remember tiles we're seen
					gdata->_map->addToMemoryMap(mpt.first, mpt.second);

					//	Surfaces will override tiles
					auto sf = gdata->_map->getSurface(mpt);
					if (sf != SURFACE__NONE)
					{
						gl = getSurfaceGlyph(sf);
						col = getSurfaceColor(sf);
					}

					//	certain tiles can also have a flickering effect
					if (doesMaptileFlicker(tl) || sf != SURFACE__NONE)
					{
						float f = 1.0f + (float)randint(-3, 3) * 0.05f;
						col.scaleHSV(1.0f, f);
					}

					//	Items override surfaces
					auto it = gdata->_map->getItem(mpt);
					if (it != nullptr)
					{
						gl = it->getGlyph();
						col = it->getColor();
					}

					//	Darken tiles that are further away.
					auto dist = hypot(mpt.first - ctr.first, mpt.second - ctr.second);
					auto darken_factor = 1.0f - 0.04f * dist;
					if (darken_factor < 0.1f) darken_factor = 0.1f;
					col.scaleHSV(1.0f, darken_factor);
				}

				//	if not, we can still see it if it's in the memory map
				else if (gdata->_map->inMemoryMap(mpt.first, mpt.second))
				{
					col.scaleHSV(1.0f, 0.09f);
					bgcol.scaleHSV(1.0f, 0.09f);
				}

				//	otherwise, we can't see it at all.
				else
				{
					gl = ' ';
					col = COLOR_BLACK;
					bgcol = COLOR_BLACK;
				}
			}

			//	Update the map info.
			_visibleGlyphs[dx][dy] = gl;
			_visibleColors[dx][dy] = col;
			_visibleBgcolors[dx][dy] = bgcol;
		}
	}


	//	Stairs.
	for (auto st : *gdata->_map->getAllStairs())
	{
		auto dpt = mapToDisplayCoord(ctr, st->_pos.first, st->_pos.second);
		if (isPointOnDisplay(dpt))
		{
			_visibleGlyphs[dpt.first][dpt.second] = st->_glyph;
			_visibleColors[dpt.first][dpt.second] = COLOR_WHITE;
		}
	}


	//	Add creatures.
	for (auto cr : gdata->_map->getAllCreatures())
	{
		//	Test whether the monster is currently onscreen
		auto dpt = mapToDisplayCoord(ctr, cr->_pos.first, cr->_pos.second);
		if (isPointOnDisplay(dpt))
		{
			//	We can always see ourselves; test whether we can see monsters
			if (canPlayerSeeCreature(gdata, cr))
			{
				//	map data
				_visibleGlyphs[dpt.first][dpt.second] = cr->getGlyph();
				_visibleColors[dpt.first][dpt.second] = cr->getColor();

				//	remember monsters we see
				if (!cr->isPlayer())
					gdata->_visibleMonsters.push_back(cr);
			}
		}
	}


	//	Cursor highlighting, if the cursor is on
	if (inCursorState(gdata))
	{
		auto dpt = mapToDisplayCoord(ctr, gdata->_cursorPt.first, gdata->_cursorPt.second);
		if (isPointOnDisplay(dpt))
			_visibleBgcolors[dpt.first][dpt.second] = COLOR_HIGHLIGHT;
	}


	//	Animations
	for (auto anim : gdata->_animations)
	{
		auto pts = anim->getPoints();
		for (unsigned i = 0; i < pts.size(); i++)
		{
			auto dpt = mapToDisplayCoord(ctr, pts[i].first, pts[i].second);
			if (isPointOnDisplay(dpt))
			{
				_visibleGlyphs[dpt.first][dpt.second] = anim->getGlyph(i);
				_visibleColors[dpt.first][dpt.second] = anim->getColor(i);
			}
		}
	}
}


//	This is the MAIN DISPLAY, with the map and stuff.
void display::drawMainInterface(gamedataPtr gdata)
{
	drawRXPImage(_img_Main, 0, 0);

	//	map name
	_win.write(3, 1, centreText(gdata->_map->getName(), 27), COLOR_LIGHT);
	_win.write(50, 1, to_string((gdata->_depth) * 10) + "'", COLOR_LIGHT);

	//	the map; check for filters first
	bool underwater_filter = gdata->_map->getTile(gdata->_player->_pos) == TILE_WATER_DEEP;
	for (unsigned x = 0; x < MAP_X_SIZE; x++)
	{
		for (unsigned y = 0; y < MAP_Y_SIZE; y++)
		{
			//	apply filters, if any
			auto col = _visibleColors[x][y];
			if (underwater_filter)
				col = col.lerp(col, TCODColor::blue, 0.5);

			//	actually show this
			_win.writec(x + MAP_X_OFFSET, y + MAP_Y_OFFSET, _visibleGlyphs[x][y], col, _visibleBgcolors[x][y]);
		}
	}

	drawSidebar(gdata);
	if (gdata->_state == STATE_LOOK)
		drawMouseover(gdata);
	else
		drawMessages(gdata);
}


void display::drawSidebar(gamedataPtr gdata)
{
	auto p = gdata->_player;
	
	int hp_per = p->getHealthPercent();
	writeFormatted(58, 4, "HEALTH    #" + to_string(p->getHealthLeft()) + "@/" + to_string(p->getMaxHealth()), { getPercentColor(hp_per) });

	_win.write(58, 5, "SURGES", COLOR_MEDIUM);
	drawProgressDots(68, 5, p->getHealingSurgesLeft(), p->getMaxHealingSurges(), COLOR_WHITE);

	if (!p->atMaxHealingSurges())
	{
		auto hcol = TCODColor::lightGrey;
		hcol = hcol.lerp(TCODColor::darkGrey, hcol, (float)p->getHealingSurgeProgress() / 100.0f);
		_win.writec(68 + p->getHealingSurgesLeft(), 5, 9, hcol);
	}

	_win.write(58, 6, "MAGIC PTS ", COLOR_MEDIUM);
	drawProgressDots(68, 6, p->getMagicPointsLeft(), p->getMaxMagicPoints(), TCODColor::lightBlue);

	writeFormatted(80, 4, "LV #" + to_string(p->_level), { COLOR_LIGHT });
	writeFormatted(80, 5, "XP #" + to_string(getXPAsPercent(gdata)) + "%", { COLOR_LIGHT });
	writeFormatted(80, 6, "AU#$" + to_string(gdata->_gold), { TCODColor::gold });
	
	for (unsigned i = 0; i < ATTR__MAX; i++)
	{
		auto attr = static_cast<Attribute>(i);
		_win.write(58, 8 + i, getAttributeAbbr(attr), COLOR_MEDIUM);
		_win.write(62, 8 + i, extendInteger(p->getDerivedAttributeValue(attr), 2), COLOR_LIGHT);
	}

	writeFormatted(68, 8,  "ACC #" + plusminus(p->getMeleeAccuracy()) + "@/#" + plusminus(p->getRangedAccuracy()), { COLOR_LIGHT, COLOR_LIGHT });
	writeFormatted(68, 9,  "EV  #" + to_string(p->getEvasion()), { COLOR_LIGHT });
	writeFormatted(68, 10, "BL  #" + to_string(p->getBlockValue()), { COLOR_LIGHT });
	writeFormatted(68, 11, "DR  #" + to_string(p->getDamageResist()), { COLOR_LIGHT });

	const vector<itemPtr> ilist = { p->_weapon, p->_ranged, p->_ammo, p->_shield, p->_armour };
	for (unsigned i = 0; i < ilist.size(); i++)
	{
		const int y = 15 + i * 2;
		if (ilist[i] != nullptr)
		{
			_win.writec(58, y, ilist[i]->getGlyph(), ilist[i]->getColor());
			_win.write(60, y, ilist[i]->getNameWithAmt(), ilist[i]->getColor());
		}
		else
			_win.write(60, y, "---", TCODColor::darkGrey);
	}

	//	active afflictions/buffs
	int y = 26;
	if (p->isGrappled())
		_win.write(58, ++y, "Grappled!", COLOR_WHITE);
	for (unsigned i = 0; i < AFFL__NONE; i++)
	{
		auto af = static_cast<Affliction>(i);
		if (p->hasAffliction(af))
			writeFormatted(58, ++y, extendInteger(p->getAfflictionStacks(af), 3) + " #" + getAfflictionName(af), { getAfflictionColor(af) });
	}
	for (unsigned i = 0; i < COND__NONE; i++)
	{
		auto cond = static_cast<Condition>(i);
		if (p->hasCondition(cond))
			writeFormatted(58, ++y, extendInteger(p->getConditionDuration(cond), 3) + " #" + getConditionName(cond), { getConditionColor(cond) });
	}
	for (unsigned i = 0; i < BUFF__NONE; i++)
	{
		auto bf = static_cast<BuffType>(i);
		if (p->hasBuff(bf))
			writeFormatted(58, ++y, extendInteger(p->getBuffDuration(bf), 3) + " #" + getBuffName(bf), { getBuffColor(bf) });
	}


	//	List of visible monsters
	y = 50;
	for (auto mon : gdata->_visibleMonsters)
	{
		_win.writec(58, ++y, mon->getGlyph(), mon->getColor());
		_win.write(60, y, mon->getName(), mon->getColor());
		_win.write(83, y, to_string(mon->getHealthPercent()) + "%", TCODColor::red);
	}
}


void display::drawMessages(gamedataPtr gdata)
{
	for (unsigned i = 0; i < gdata->_messages.size(); i++)
	{
		auto col = gdata->_messages[i]._color;
		col.scaleHSV(1.0f, 1.0f - 0.05f * (float)i);
		_win.write(4, 48 + i, gdata->_messages[i]._txt, col);
	}
}



//	Stuff under the cursor
void display::drawMouseover(gamedataPtr gdata)
{
	//	tile
	auto tl = gdata->_map->getTile(gdata->_cursorPt);
	_win.writec(4, 48, getMaptileGlyph(tl), getMaptileColor(tl));
	_win.write(6, 48, getMaptileName(tl), getMaptileColor(tl));

	//	surface
	auto sf = gdata->_map->getSurface(gdata->_cursorPt);
	if (sf != SURFACE__NONE)
	{
		_win.writec(30, 48, getSurfaceGlyph(sf), getSurfaceColor(sf));
		_win.write(32, 48, getSurfaceName(sf), getSurfaceColor(sf));
	}

	//	item, if any
	auto it = gdata->_map->getItem(gdata->_cursorPt);
	if (it != nullptr)
	{
		_win.writec(4, 49, it->getGlyph(), it->getColor());
		_win.write(6, 49, it->getVerboseName(), it->getColor());
	}

	//	stairs, if any
	auto st = gdata->_map->getStairs(gdata->_cursorPt);
	if (st != nullptr)
	{
		_win.writec(4, 50, '>', COLOR_WHITE);
		_win.write(6, 50, st->_name, COLOR_WHITE);
	}


	//	is there a creature here?
	auto t = gdata->_map->getCreature(gdata->_cursorPt);
	if (t != nullptr && !t->isPlayer())
		drawCreatureInfo(gdata, t, 4, 54);
}


//	Info about an NPC
void display::drawCreatureInfo(gamedataPtr gdata, creaturePtr t, int atx, int aty)
{
	_win.writec(atx, aty, t->getGlyph(), t->getColor());
	_win.write(atx + 2, aty, t->getName(), t->getColor());

	writeFormatted(atx + 40, aty, "HP #" + to_string(t->getHealthLeft()), { TCODColor::red });

	//	defensive stats
	aty += 2;
	writeFormatted(atx, aty, "EV #" + to_string(t->getEvasion()) + " @BL #" + to_string(t->getBlockValue()) + " @DR #" + to_string(t->getDamageResist()),
		{ COLOR_LIGHT, COLOR_LIGHT, COLOR_LIGHT });

	//	attacks
	aty += 2;
	auto dam = t->getMeleeDamageDice();
	auto dbonus = t->getMeleeDamageBonus();
	auto damstr = "melee attack: #" + plusminus(t->getMeleeAccuracy()) + " " + to_string(dam.first) + "d" + to_string(dam.second);
	if (dbonus != 0)
		damstr += plusminus(dbonus);
	writeFormatted(atx, aty, damstr, { COLOR_LIGHT });

	//	current afflictions
	aty += 2;
	int xplus = 0;
	for (unsigned i = 0; i < AFFL__NONE; i++)
	{
		auto af = static_cast<Affliction>(i);
		if (t->hasAffliction(af))
		{
			_win.write(atx + xplus, aty, getAfflictionName(af), getAfflictionColor(af));
			xplus += getAfflictionName(af).size() + 1;
		}
	}

	writeFormatted(4, 62, "#v @View detailed info", { COLOR_LIGHT });
}


//	Checks that a given display coordinate is in the bounds of the display.
bool display::isPointOnDisplay(const intpair pt)
{
	return pt.first >= 0 && pt.first < MAP_X_SIZE && pt.second >= 0 && pt.second < MAP_Y_SIZE;
}


//	Inverse of the function below: this takes a MAP coordinate and returns the coordinate at which it should
//	appear on the DISPLAY. Both "ctr" and "x","y" are map coordinates.
//	Make sure to check that the returned coordinate is actually IN the display, as some map coordinates will
//	be outside the display area if the map is larger than the window.
intpair display::mapToDisplayCoord(const intpair ctr, int x, int y)
{
	intpair pt;
	pt.first = (MAP_X_SIZE / 2) + (x - ctr.first);
	pt.second = (MAP_Y_SIZE / 2) + (y - ctr.second);
	return pt;
}



//	This converts a coordinate from the DISPLAY GRID, starting at 0,0 in the top left of the display window
//	to its corresponding coordinate on the MAP, where the given point "ctr" appears at the CENTRE of the display.
//	"ctr" is in MAP coordinates. "x" and "y" are the DISPLAY coordinates to convert.
intpair display::displayToMapCoord(const intpair ctr, int x, int y)
{
	intpair pt;
	pt.first = x + (ctr.first - MAP_X_SIZE / 2);
	pt.second = y + (ctr.second - MAP_Y_SIZE / 2);
	return pt;
}


//	A colour that ranged from 0 to 100, spanning from red to yellow to green
colorType display::getPercentColor(int per) const
{
	//	cap at the limits of the percent range
	if		(per < 0) per = 0;
	else if (per > 100) per = 100;
	return _percentColors[per];
}


//	Colors assigned to various tiers of resistance, which can range from negative to positive values
colorType display::getResistantValueColor(int val) const
{
	if		(val < 0)		return TCODColor::red;
	else if (val <= 25)		return TCODColor::darkYellow;
	else if (val <= 50)		return TCODColor::green;
	else if (val < 100)		return TCODColor::lightGreen;
	else					return TCODColor::green;
}


//	Advances animations, deletes any that have expired.
void display::tickAnimations(gamedataPtr gdata)
{
	vector<animationPtr> toRemove;
	for (auto anim : gdata->_animations)
	{
		anim->tick();
		if (anim->isComplete())
			toRemove.push_back(anim);
	}
	for (auto anim : toRemove)
	{
		auto f = find(gdata->_animations.begin(), gdata->_animations.end(), anim);
		gdata->_animations.erase(f);
	}
}


//	Text with colour formatting. Initially uses a default colour.
//	Upon reaching a '#' character, switches to the next colour in the given colour list.
//	Upon reaching a '@' character, reverts to the default colour.
void display::writeFormatted(int atx, int aty, string txt, vector<colorType> colors, float darkenFactor)
{
	colorType col = COLOR_MEDIUM;
	int colIdx = 0;
	int xp = 0;

	//	if a darkening-factor is given, we adjust all colors by that factor
	if (darkenFactor != 0.0f)
		col.scaleHSV(1.0f, darkenFactor);

	//	draw the message
	for (int i = 0; i < txt.size(); i++)
	{
		if (txt[i] == MSG_COLOR_CHANGE_MARK)
		{
			if (colIdx < colors.size())
			{
				col = colors[colIdx];
				if (darkenFactor != 0.0f)
					col.scaleHSV(1.0f, darkenFactor);
				colIdx++;
			}

			//	hopefully switching to this awful colour will indicate that there's an error
			else
			{
				//cout << "ERROR: Badly formatted message '" + txt + "'" << endl;
				col = TCODColor::lightestPink;
			}
		}
		else if (txt[i] == MSG_DEFAULT_COLOR_MARK)
		{
			col = COLOR_MEDIUM;
			if (darkenFactor != 0.0f)
				col.scaleHSV(1.0f, darkenFactor);
		}
		else
			_win.writec(atx + xp++, aty, txt[i], col);
	}
}


//	Writes 'title' at x,y and 'val' and x2,y with dots connecting them
void display::drawDottedLinePair(int x, int y, int x2, const string title, const string val, colorType val_col, colorType title_col)
{
	//	the two values
	_win.write(x, y, title, title_col);
	_win.write(x2, y, val, val_col);

	//	the dots
	for (unsigned x3 = x + title.size(); x3 < x2; x3++)
		_win.writec(x3, y, 249, COLOR_DARK);
}


//	Dot-like glyphs form a progress bar, with current value 'filled', and a total of 'maxval' dots drawn.
//	'col' is the colour of filled dots; unfilled dots are grey.
void display::drawProgressDots(int x, int y, int filled, int maxval, colorType col)
{
	for (unsigned i = 0; i < filled; i++)
		_win.writec(x + i, y, 9, col);
	for (unsigned i = filled; i < maxval; i++)
		_win.writec(x + i, y, 9, TCODColor::darkGrey);
}

void display::drawLineHorizontal(int x, int y, int width, colorType color)
{
	for (int i = 0; i < width; i++)
		_win.writec(x + i, y, 196, color);
}

void display::drawLineVertical(int x, int y, int height, colorType color)
{
	for (int i = 0; i < height; i++)
		_win.writec(x, y + i, 179, color);
}
