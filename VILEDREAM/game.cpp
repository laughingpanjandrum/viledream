#include "game.h"



game::game() :
	_isGameOver(false)
{
}

void game::start()
{
	//	seed the RNG
	initrand();

	//	create basic elements
	_ih = inputHandlerPtr(new inputHandler());
	_gdata = gamedataPtr(new gamedata());
	_ldata = loadedFileDataPtr(new loadedFileData());


	//	load file data
	dataLoader::loadAll(_ldata);


	//	player and items
	initializeEquipmentStorage(_gdata);
	_gdata->_player = playerPtr(new player());
	_gdata->_player->_weapon = itemPtr(new item(itemData::WEAPON_DAGGER));
	addToInventory(_gdata, itemPtr(new item(itemData::SCROLL_PORTAL)));
	//_gdata->_player->_leftRing = itemPtr(new item(itemData::RING_BLOOD_HUNGER));
	//addToInventory(_gdata, itemPtr(new item(itemData::COATING_POISON, 5)));
	//_gdata->_player->_ranged = itemPtr(new item(itemData::RANGED_SHORTBOW));
	//_gdata->_player->_ammo = itemPtr(new item(itemData::ARROWS_STANDARD, 100));


	//	map
	_gdata->_map = mapgen::generate_type2(_ldata, ZONE_ID_FORSAKEN_PRISON, 1); //(_ldata, ZONE_ID_FORSAKEN_PRISON, 1, false);
	_gdata->_map->addCreature(_gdata->_player, _gdata->_map->_startPt);
	_gdata->_map->setFOVMapCentre(_gdata->_player->_pos);


	//	permanent home base map, and general zone progress
	_gdata->_homeBase = mapgen::generate_HomeBase();


	//	run the game
	_globalTickCounter = 0;
	_gdata->_depth = 1;
	_gdata->_state = STATE_NORMAL;
	mainGameLoop();
}

void game::mainGameLoop()
{
	while (!_isGameOver)
	{
		drawScreen();
		processInput();
		while (!_gdata->_player->isActionDelayClear())
			tick();
	}
}

void game::drawScreen()
{
	_disp.clear();

	switch (_gdata->_state)
	{
	case(STATE_CHARACTER_SHEET):
		_disp.drawCharacterSheet(_gdata); break;

	case(STATE_EXAMINE_MONSTER):
		_disp.drawMonsterInfo(_gdata, static_pointer_cast<monster>(_gdata->_target)); break;

	case(STATE_VIEW_INVENTORY):
		_disp.drawInventory(_gdata, _gdata->_invCategory); break;
	
	case(STATE_SELECT_ITEM_TO_USE):
		_disp.drawInventory(_gdata, ITEM_CONSUMABLE); break;

	case(STATE_SELECT_ITEM_TO_THROW):
		_disp.drawInventory(_gdata, ITEM_THROWN); break;

	case(STATE_MODIFY_EQUIPMENT):
	case(STATE_VIEW_EQUIPMENT):
		_disp.drawEquipment(_gdata); break;

	case(STATE_SELECT_ITEM_TO_EQUIP):
		_disp.drawInventory(_gdata, _gdata->_invCategory, false);
		break;

	case(STATE_SELECT_SPELL_TO_CAST):
	case(STATE_VIEW_SPELLS):
		_disp.drawSpellList(_gdata); break;

	case(STATE_PICKUP_ITEM):
		_disp.drawEquipmentPickup(_gdata, _gdata->_pickingUp); break;

	case(STATE_VIEW_SHOP):
		_disp.drawShop(_gdata, _gdata->_currentShop); break;

	case(STATE_VIEW_FEATS):
		_disp.drawFeatsKnown(_gdata); break;
	case(STATE_LEARN_FEAT):
		_disp.drawFeatsToBuy(_gdata); break;

	case(STATE_RAISE_ATTRIBUTE):
		_disp.drawAttributeSelection(_gdata); break;

	default:
		_disp.drawMainInterface(_gdata);
	}

	_disp.refresh();
	_disp.tickAnimations(_gdata);
}


void game::processInput()
{
	//	check for special state-changing triggers
	if (_gdata->_triggerRaiseAttribute)
	{
		_gdata->_state = STATE_RAISE_ATTRIBUTE;
		_gdata->_triggerRaiseAttribute = false;
	}


	//	Check for automatic behaviours, which override input.
	if (_gdata->_automoveDist > 0)
		doAutomove(_gdata);


	//	Otherwise, test for input normally.
	else
	{
		_ih->checkForKeypress();

		//	we can quit whenever we want
		if (_ih->isKeyPressed('q') && _ih->isCtrlPressed())
			_isGameOver = true;

		//	we can always go back
		else if (_ih->isKeyPressed(TCODK_ESCAPE))
			backOut();


		//	What happens depends on game state
		switch (_gdata->_state)
		{
			//	Aiming a ranged weapon
		case(STATE_AIM_RANGED):
		case(STATE_AIM_THROWN):
			if (_ih->isDirectionalKeyPressed())
				moveCursor(_gdata, _ih->getVectorFromKeypress());
			else if (_ih->isKeyPressed('f') && _gdata->_state == STATE_AIM_RANGED)
				playerTriggerRangedAttack(_gdata, _gdata->_cursorPt);
			else if (_ih->isKeyPressed('t') && _gdata->_state == STATE_AIM_THROWN)
				selectItemToThrow(_gdata, _gdata->_throwing);
			break;


			//	Looking around
		case(STATE_LOOK):
			if (_ih->isDirectionalKeyPressed())
				moveCursor(_gdata, _ih->getVectorFromKeypress());
			else if (_ih->isKeyPressed('v'))
				tryExamineMonster(_gdata);
			break;


			//	Using items
		case(STATE_SELECT_ITEM_TO_USE):
			if (_ih->isKeypressACharacter())
				playerTryUseConsumable(_gdata, _ih->getIntFromKeypressChar());
			break;
		case(STATE_SELECT_ITEM_TO_THROW):
			if (_ih->isKeypressACharacter())
				playerAimThrown(_gdata, _ih->getIntFromKeypressChar());
			break;


			//	Inventory
		case(STATE_VIEW_INVENTORY):
			if (_ih->isKeypressACharacter())
				_gdata->_idx = _ih->getIntFromKeypressChar();
			else if (_ih->isKeyPressed(TCODK_RIGHT))
				cycleInventoryCategory(_gdata, 1);
			else if (_ih->isKeyPressed(TCODK_LEFT))
				cycleInventoryCategory(_gdata, -1);
			else if (_ih->isKeyPressed(TCODK_ENTER))
				tryActivateFromInventory(_gdata, _gdata->_idx, _gdata->_invCategory);
			break;


			//	Spell list
		case(STATE_VIEW_SPELLS):
			if (_ih->isKeypressACharacter())
				_gdata->_idx = _ih->getIntFromKeypressChar();
			else if (_ih->isKeyPressed(TCODK_ENTER))
				selectSpellToCast(_gdata, _gdata->_idx);
			break;
		case(STATE_SELECT_SPELL_TO_CAST):
			if (_ih->isKeypressACharacter())
				selectSpellToCast(_gdata, _ih->getIntFromKeypressChar());
			break;
		case(STATE_AIM_SPELL):
			if (_ih->isDirectionalKeyPressed())
				triggerSpellInDirection(_gdata, _gdata->_casting, _ih->getVectorFromKeypress());
			break;


			//	Picking stuff up
		case(STATE_PICKUP_ITEM):
			if (_ih->isKeyPressed('g'))
				pickupItem(_gdata, _gdata->_pickingUp, 'g');
			else if (_ih->isKeyPressed('e'))
				pickupItem(_gdata, _gdata->_pickingUp, 'e');
			else if (_ih->isKeyPressed('q'))
				pickupItem(_gdata, _gdata->_pickingUp, 'q');
			break;


			//	Leveling
		case(STATE_RAISE_ATTRIBUTE):
			if (_ih->isKeypressACharacter())
				playerRaiseAttribute(_gdata, _ih->getIntFromKeypressChar());
			break;


			//	Shopping
		case(STATE_VIEW_SHOP):
			if (_ih->isKeypressACharacter())
				_gdata->_idx = _ih->getIntFromKeypressChar();
			else if (_ih->isKeyPressed(TCODK_ENTER))
				tryBuyCurrentItem(_gdata);
			break;


			//	Feats
		case(STATE_LEARN_FEAT):
		case(STATE_VIEW_FEATS):
			if (_ih->isKeypressACharacter())
				_gdata->_idx = _ih->getIntFromKeypressChar();
			else if (_ih->isKeyPressed(TCODK_ENTER))
			{
				if (_gdata->_state == STATE_VIEW_FEATS)
					learnFeatForSlot(_gdata, _gdata->_idx);
				else
					equipNewFeatInSlot(_gdata, _gdata->_idx, _gdata->_lastIdx);
			}
			break;


			//	Equipment
		case(STATE_MODIFY_EQUIPMENT):
		case(STATE_VIEW_EQUIPMENT):
			if (_ih->isKeyPressed('R') && _gdata->_state == STATE_MODIFY_EQUIPMENT)
				selectItemToUnequip(_gdata, _gdata->_idx);
			else if (_ih->isKeypressACharacter())
				_gdata->_idx = _ih->getIntFromKeypressChar();
			else if (_ih->isKeyPressed(TCODK_ENTER) && _gdata->_state == STATE_MODIFY_EQUIPMENT)
				selectEquipmentSlot(_gdata, _gdata->_idx);
			
			break;

		case(STATE_SELECT_ITEM_TO_EQUIP):
			if (_ih->isKeypressACharacter())
				_gdata->_idx = _ih->getIntFromKeypressChar();
			else if (_ih->isKeyPressed(TCODK_ENTER))
				selectItemToEquip(_gdata, _gdata->_idx);
			break;


			//	Normal game events
		case(STATE_NORMAL):
			mainGameInput();
			break;
		}
	}

	//	always update map data afterward
	_disp.updateVisibleMapData(_gdata);
}


//	Input while in 'NORMAL' state
void game::mainGameInput()
{
	//	movement
	if (_ih->isDirectionalKeyPressed())
	{
		if (_ih->isShiftKeyPressed())
			startAutoMoving(_gdata, _ih->getVectorFromKeypress());
		else if (_ih->isCtrlPressed())
			triggerReachAttack(_gdata, _gdata->_player, _ih->getVectorFromKeypress());
		else
			doPlayerMove(_gdata, _ih->getVectorFromKeypress());
	}

	//	aiming attacks
	else if (_ih->isKeyPressed('f'))
		playerAimRanged(_gdata);
	else if (_ih->isKeyPressed('t'))
		openThrownSelectMenu(_gdata);
	else if (_ih->isKeyPressed('c'))
		openSpellSelectMenu(_gdata);

	//	swap weapons
	else if (_ih->isKeyPressed('q'))
		playerSwapWeapons(_gdata);

	//	rest one turn
	else if (_ih->isKeyPressed('z'))
		_gdata->_player->setActionDelay();

	//	heal ourself
	else if (_ih->isKeyPressed('s'))
		playerUseHealingSurge(_gdata);

	//	look around
	else if (_ih->isKeyPressed('x'))
		enterLookMode(_gdata);

	//	get item
	else if (_ih->isKeyPressed('g'))
		tryPickupItem(_gdata);

	//	use item
	else if (_ih->isKeyPressed('a'))
		openConsumableSelectMenu(_gdata);

	//	use some stairs
	else if (_ih->isKeyPressed('>') || _ih->isKeyPressed('<'))
		tryDescend();

	//	viewing menus
	else if (_ih->isKeyPressed('@'))
		_gdata->_state = STATE_CHARACTER_SHEET;
	else if (_ih->isKeyPressed('i'))
		openInventory(_gdata);
	else if (_ih->isKeyPressed('e'))
		openEquipmentMenu(_gdata, false);
	else if (_ih->isKeyPressed('m'))
		openSpellList(_gdata);
	else if (_ih->isKeyPressed('F'))
		openFeatsKnown(_gdata);

	//	debug
	else if (_ih->isKeyPressed('`'))
		awaitDebugCommand();
}


//	Back out of whatever interface we're in, closer to the main display
void game::backOut()
{
	//	Can't back out of this menu, you have to choose something first!
	if (_gdata->_state == STATE_RAISE_ATTRIBUTE)
		messages::add(_gdata, "Select an attribute to raise!", COLOR_WHITE);

	//	Return to equipment list
	else if (_gdata->_state == STATE_SELECT_ITEM_TO_EQUIP)
		openEquipmentMenu(_gdata, true);

	//	Go back to the normal interface
	else
		_gdata->_state = STATE_NORMAL;
}


//	Navigate the given stairs to a new map.
void game::moveToNewMap(mapStairsPtr st)
{
	messages::add(_gdata, "You descend...", COLOR_WHITE);
	drawScreen();

	//	if this is the home base map, we don't generate an entirely new map
	if (st->_mapId == ZONE_ID_HOME_BASE)
	{
		//mapgen::refreshHomeMapStairs(_gdata->_homeBase, st->_dangerLevel);
		_gdata->_map = _gdata->_homeBase;
		_gdata->_depth = 0;
	}

	//	generate the new map
	else
	{
		_gdata->_map = mapgen::generate_type2(_ldata, st->_mapId, st->_dangerLevel);
		_gdata->_depth = st->_dangerLevel;
	}

	//	put us on the map
	_gdata->_map->addCreature(_gdata->_player, _gdata->_map->_startPt);
	_gdata->_map->setFOVMapCentre(_gdata->_player->_pos);
}


//	If there are stairs under us, use them
void game::tryDescend()
{
	auto st = _gdata->_map->getStairs(_gdata->_player->_pos);
	if (st != nullptr)
		moveToNewMap(st);
	else
		messages::add(_gdata, "No stairs here!", COLOR_NEGATIVE);
}



//	Adds any monsters waiting to spawn, then clears the spawn list.
void game::triggerMonsterSpawns()
{
	//	do all the spawns
	for (unsigned i = 0; i < _gdata->_triggerMonsterSpawnId.size(); i++)
	{
		auto mdat = mapgen::getMonsterDataWithId(&_ldata->_monsterData, _gdata->_triggerMonsterSpawnId[i]);
		if (mdat != nullptr)
		{
			//	Only spawn if the target point is unoccupied
			auto pt = _gdata->_triggerMonsterSpawnPt[i];
			if (_gdata->_map->getCreature(pt) == nullptr)
			{
				auto mon = monsterPtr(new monster(mdat->_tokens));
				mon->_damageTaken = _gdata->_triggerMonsterSpawnDamage[i];
				_gdata->_map->addCreature(mon, pt);
			}
		}
		else
			cout << "ERROR: Trying to spawn monster with id " << _gdata->_triggerMonsterSpawnId[i] << "; that id doesn't exist" << endl;
	}

	//	clear the queues
	_gdata->_triggerMonsterSpawnId.clear();
	_gdata->_triggerMonsterSpawnPt.clear();
	_gdata->_triggerMonsterSpawnDamage.clear();
}


//	Events that happen to a creature on each global tick.
void game::doCreatureTick(creaturePtr cr)
{
	//	affliction ticks
	if (cr->hasAffliction(AFFL_BURN))
		creatureTakeDamage(_gdata, cr, randint(2, 5));
	if (cr->hasAffliction(AFFL_POISON))
		creatureTakeDamage(_gdata, cr, randint(1, 2));

	//	reduce duration of effects
	cr->tickAfflictions();
	cr->tickBuffs();
	cr->tickConditions();
	cr->verifyGrappler();

	//	tile effects
	standOnTile(_gdata, cr);

	//	player regens MP
	if (cr->isPlayer())
		_gdata->_player->tickMpRegen();
}


//	Advances time by 1 tick. AIs get turns if they come up.
void game::tick()
{
	//	count down to a global tick
	_globalTickCounter++;
	bool doGlobalTick = false;
	if (_globalTickCounter >= ACTION_DELAY_NORMAL)
	{
		_globalTickCounter = 0;
		doGlobalTick = true;
		_gdata->_map->tickSurfaces();
	}

	//	advance creature timers
	for (auto cr : _gdata->_map->getAllCreatures())
	{
		cr->tickActionDelay();
		if (doGlobalTick)
			doCreatureTick(cr);
		if (!cr->isPlayer() && cr->isActionDelayClear())
			ai::takeTurn(_gdata, static_pointer_cast<monster>(cr));
	}

	//	spawn new monsters, if there are any
	triggerMonsterSpawns();
}


void game::awaitDebugCommand()
{
	auto cmd = _disp.getstr(1, 1, "enter debug command: ");
	if (cmd == "spawn")
	{
		auto id = _disp.getstr(1, 2, " enter monster id: ");
		auto mdat = mapgen::getMonsterDataWithId(&_ldata->_monsterData, id);
		if (mdat != nullptr)
		{
			auto pts = getAdjacentFreePoints(_gdata, _gdata->_player, _gdata->_player->_pos);
			if (!pts.empty())
				_gdata->_map->addCreature(monsterPtr(new monster(mdat->_tokens)), pts[0]);
		}
		else
			messages::add(_gdata, "Invalid monster id: " + id, COLOR_NEGATIVE);
	}
	else if (cmd == "level")
		playerAddLevel(_gdata);
}
