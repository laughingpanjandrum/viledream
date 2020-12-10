#include "manageItems.h"


//	Figures out what the effect of a given consumable is and DOES the effect.
void applyConsumableEffect(gamedataPtr gdata, itemData::itid id)
{
	string mtxt = "You used " + itemData::getName(id) + ".";
	switch (id)
	{
	case(itemData::COATING_FLAME):
		gdata->_player->addBuff(BUFF_COATING_FLAME, 25);
		mtxt = "Your weapon bursts into flames!";
		break;

	case(itemData::COATING_ICE):
		gdata->_player->addBuff(BUFF_COATING_FROST, 25);
		mtxt = "Your weapon turns to ice!";
		break;

	case(itemData::COATING_LIGHTNING):
		gdata->_player->addBuff(BUFF_COATING_LIGHTNING, 25);
		mtxt = "Your weapon crackles with electricity!";
		break;

	case(itemData::COATING_POISON):
		gdata->_player->addBuff(BUFF_COATING_POISON, 40);
		mtxt = "Your weapon drips with poison!";
		break;

	case(itemData::POTION_HASTE):
		gdata->_player->addBuff(BUFF_HASTE, 50);
		mtxt = "You feel fast!";
		break;

	case(itemData::POTION_MIGHT):
		gdata->_player->addBuff(BUFF_MIGHT, 100);
		mtxt = "Your muscles bulge to absurd proportion!";
		break;

	case(itemData::POTION_STONESKIN):
		gdata->_player->addBuff(BUFF_STONESKIN, 100);
		mtxt = "Stone scales form on your skin!";
		break;

	case(itemData::SCROLL_PORTAL):
		gdata->_map->addStairsToHomeBase(gdata->_player->_pos);
		mtxt = "A staircase appears beneath you!";
		break;

	default:
		cout << "ERROR: No effect for item " << itemData::getName(id) << endl;
	}

	//	special message about it
	messages::add(gdata, mtxt, itemData::getColor(id));
}


//	Attempt to use the consumable at the given index.
void playerTryUseConsumable(gamedataPtr gdata, const int idx)
{
	//	Is the index valid?
	if (idx >= 0 && idx < gdata->_carriedConsumables.size())
	{
		//	apply its effect, expend it
		auto it = gdata->_carriedConsumables[idx];
		applyConsumableEffect(gdata, it->_id);
		expendItem(gdata, it, &gdata->_carriedConsumables);
		
		//	return to normal
		gdata->_state = STATE_NORMAL;
	}
}


//	Selecting a consumable to use.
void openConsumableSelectMenu(gamedataPtr gdata)
{
	gdata->_state = STATE_SELECT_ITEM_TO_USE;
	gdata->_idx = -1;
	messages::add(gdata, "Select an item to use...", COLOR_WHITE);
}


//	Throw an item from our inventory.
void selectItemToThrow(gamedataPtr gdata, itemPtr it)
{
	triggerThrownAttack(gdata, gdata->_player, it, gdata->_cursorPt);
	gdata->_player->setActionDelay();
	gdata->_state = STATE_NORMAL;
	expendItem(gdata, it, &gdata->_carriedBombs);
}


void openThrownSelectMenu(gamedataPtr gdata)
{
	gdata->_state = STATE_SELECT_ITEM_TO_THROW;
	gdata->_idx = -1;
	messages::add(gdata, "Select an item to throw...", COLOR_WHITE);
}


//	Attemt to ACTIVATE an item from inventory, which has a different effect based on its type:
//		consumables are eaten
//		ammo is equipped
//		thrown items are prepped to throw
void tryActivateFromInventory(gamedataPtr gdata, const int idx, ItemCategory cat)
{
	auto ilist = getItemListForCategory(gdata, cat);
	if (idx >= 0 && idx < ilist->size())
	{
		auto it = ilist->at(idx);
		switch (it->_category)
		{
		case(ITEM_AMMO):		playerEquipItem(gdata, it); gdata->_state = STATE_NORMAL; break;
		case(ITEM_CONSUMABLE):	playerTryUseConsumable(gdata, idx); break;
		case(ITEM_THROWN):		playerAimThrown(gdata, idx); break;
		}
	}
}


//	Delete the entire item from the inventory list.
void loseItem(gamedataPtr gdata, itemPtr it, vector<itemPtr>* ilist)
{
	auto f = find(ilist->begin(), ilist->end(), it);
	if (f != ilist->end())
		ilist->erase(f);
	else
		cout << "ERROR: Tried to delete item " << it->getName() << " from an inventory list, but it wasn't in there!" << endl;
}


//	Reduce item quantity. If quantity reaches zero, delete the item from the item list.
void expendItem(gamedataPtr gdata, itemPtr it, vector<itemPtr>* ilist)
{
	it->_amount--;
	if (it->_amount < 1)
		loseItem(gdata, it, ilist);
}

void cycleInventoryCategory(gamedataPtr gdata, const int vec)
{
	auto f = find(CONSUMABLE_CATEGORIES.begin(), CONSUMABLE_CATEGORIES.end(), gdata->_invCategory);
	if (vec > 0)
	{
		f++;
		if (f == CONSUMABLE_CATEGORIES.end())
			f = CONSUMABLE_CATEGORIES.begin();
	}
	else if (vec < 0)
	{
		if (f == CONSUMABLE_CATEGORIES.begin())
			f = CONSUMABLE_CATEGORIES.end() - 1;
		else
			f--;
	}
	gdata->_invCategory = *f;
}

void openInventory(gamedataPtr gdata)
{
	gdata->_state = STATE_VIEW_INVENTORY;
	gdata->_invCategory = ITEM_CONSUMABLE;
	gdata->_idx = -1;
}

const vector<itemPtr>* getItemListForCategory(gamedataPtr gdata, ItemCategory cat)
{
	switch (cat)
	{
	case(ITEM_AMMO):			return &gdata->_carriedAmmo;
	case(ITEM_CONSUMABLE):		return &gdata->_carriedConsumables;
	case(ITEM_THROWN):			return &gdata->_carriedBombs;
	default:					return getEquipmentList(gdata, cat);
	}
}


//	Equip a given item.
void playerEquipItem(gamedataPtr gdata, itemPtr it)
{
	//	is there an item in this slot already?
	//	if so, return it to inventory
	auto here = gdata->_player->getItemInSlot(it->_category);
	if (here != nullptr)
		addToInventory(gdata, here);

	//	equip the new item
	gdata->_player->equipItem(it);
	messages::add(gdata, "Equipped " + it->getVerboseName() + ".", it->getColor());
	gdata->_player->setActionDelay();
}


//	Equip in alternate slot.
void playerEquipAsAlt(gamedataPtr gdata, itemPtr it)
{
	if (it->_category == ITEM_WEAPON)
		gdata->_player->_altWeapon = it;
	else if (it->_category == ITEM_RING)
		gdata->_player->_rightRing = it;
	messages::add(gdata, "Set " + it->getVerboseName() + " as alt.", COLOR_WHITE);
}


//	Switch to our alternate weapon.
void playerSwapWeapons(gamedataPtr gdata)
{
	swap(gdata->_player->_weapon, gdata->_player->_altWeapon);
	messages::add(gdata, "You swap your weapon!", COLOR_WHITE);
	gdata->_player->setActionDelay();
}


//	Un-equip item in the selected slot.
void selectItemToUnequip(gamedataPtr gdata, const int idx)
{
	auto ilist = gdata->_player->getAllEquippedItems();
	if (idx >= 0 && idx < ilist.size())
	{
		auto it = ilist[idx];
		if (it != nullptr)
		{
			gdata->_player->unequipItem(it);
			addToInventory(gdata, it);
		}
	}
}


//	Equip the chosen item, if possible.
void selectItemToEquip(gamedataPtr gdata, const int idx)
{
	auto ilist = getItemListForCategory(gdata, gdata->_invCategory);
	if (idx >= 0 && idx < ilist->size())
	{
		//	item to equip
		auto it = ilist->at(idx);

		//	equip the new item & return to the equipment menu.
		playerEquipItem(gdata, it);
		openEquipmentMenu(gdata, true);

		//	remove equipped item from inventory
		loseItem(gdata, it, getEquipmentList(gdata, it->_category));
	}
}


//	Select an equipment slot; we pick an item to equip in that slot.
void selectEquipmentSlot(gamedataPtr gdata, const int idx)
{
	auto cats = gdata->_player->getEquippedItemCategories();
	if (idx >= 0 && idx < cats.size())
	{
		gdata->_state = STATE_SELECT_ITEM_TO_EQUIP;
		gdata->_idx = -1;
		gdata->_invCategory = cats[idx];
		messages::add(gdata, "Select an item to equip...", COLOR_WHITE);
	}
}


//	View our equipped items. If 'allow_modify' is set, we are also allowed to change them.
void openEquipmentMenu(gamedataPtr gdata, bool allow_modify)
{
	gdata->_state = allow_modify ? STATE_MODIFY_EQUIPMENT : STATE_VIEW_EQUIPMENT;
	gdata->_idx = -1;
}


//	Each category of equippable item has its own list, arranged alphabetically.
void initializeEquipmentStorage(gamedataPtr gdata)
{
	auto cats = { ITEM_ARMOUR, ITEM_CLOAK, ITEM_RANGED, ITEM_RING, ITEM_SHIELD, ITEM_WEAPON };
	for (auto c : cats)
		gdata->_carriedEquipment.push_back({});
}


//	Returns a pointer to the equipment list for the given category of equipment.
vector<itemPtr>* getEquipmentList(gamedataPtr gdata, const ItemCategory cat)
{
	switch (cat)
	{
	case(ITEM_ARMOUR):		return &gdata->_carriedEquipment[0];
	case(ITEM_CLOAK):		return &gdata->_carriedEquipment[1];
	case(ITEM_RANGED):		return &gdata->_carriedEquipment[2];
	case(ITEM_RING):		return &gdata->_carriedEquipment[3];
	case(ITEM_SHIELD):		return &gdata->_carriedEquipment[4];
	case(ITEM_WEAPON):		return &gdata->_carriedEquipment[5];
	default:
		return nullptr;
	}
}



//	If the two items are compatible, tries to stack them together.
//	'it' will be combined into 'base', so 'it' should no longer exist afterward.
//	Returns 'True' if the items were stacked.
bool tryStackItems(gamedataPtr gdata, itemPtr base, itemPtr it)
{
	if (base->stacksWith(it.get()))
	{
		base->_amount += it->_amount;
		return true;
	}
	else
		return false;
}


//	Put an item in a list of items. If it's a stackable item, try to stack with something on the list.
void addToItemList(gamedataPtr gdata, itemPtr it, vector<itemPtr>* ilist)
{
	//	try stacking
	if (it->canStack())
	{
		for (auto other : *ilist)
		{
			if (tryStackItems(gdata, other, it))
				return;
		}
	}

	//	if we failed to stack, append item to the end
	ilist->push_back(it);
}


void addToInventory(gamedataPtr gdata, itemPtr it)
{
	//	Ammo of the same type as our existing ammo will stack with it
	if (it->_category == ITEM_AMMO && gdata->_player->_ammo != nullptr)
	{
		if (tryStackItems(gdata, gdata->_player->_ammo, it))
			return;
	}

	//	Gold goes straight into the gold pile
	else if (it->_category == ITEM_GOLD)
	{
		gdata->_gold += it->_amount;
		return;
	}

	//	Otherwise, append to one of our item lists.
	switch (it->_category)
	{
		//	carried items
	case(ITEM_AMMO):		addToItemList(gdata, it, &gdata->_carriedAmmo); break;
	case(ITEM_CONSUMABLE):	addToItemList(gdata, it, &gdata->_carriedConsumables); break;
	case(ITEM_SPELLBOOK):	playerGetSpellbook(gdata, it); break;
	case(ITEM_THROWN):		addToItemList(gdata, it, &gdata->_carriedBombs); break;

		//	equipment
	case(ITEM_ARMOUR):
	case(ITEM_CLOAK):
	case(ITEM_RANGED):
	case(ITEM_RING):
	case(ITEM_SHIELD):
	case(ITEM_WEAPON):
		addToItemList(gdata, it, getEquipmentList(gdata, it->_category)); break;
	}
}



//	Picks up an item off the ground.
//	"and_do_with" determines what we do with the item when we get it:
//		'g'		add to stash
//		'e'		equip immediately (if possible)
//		'q'		equip as our alternate weapon (if possible)
//	If an invalid 'and_do_with' is given, nothing happens.
void pickupItem(gamedataPtr gdata, itemPtr it, char and_do_with)
{
	//	What to do with the item
	switch (and_do_with)
	{
	case('g'):
		addToInventory(gdata, it);
		messages::add(gdata, "Stashed " + it->getVerboseName() + ".", COLOR_WHITE);
		break;

	case('e'):
		playerEquipItem(gdata, it);
		break;

	case('q'):
		if (!(it->_category == ITEM_WEAPON || it->_category == ITEM_RING))
			return;
		else
		{
			playerEquipAsAlt(gdata, it);
			break;
		}
	}

	//	The item is removed from the map
	gdata->_map->removeItem(gdata->_player->_pos);
	gdata->_state = STATE_NORMAL;
}


//	If there's an item at the given position, try auto-grabbing it
void tryAutopickup(gamedataPtr gdata, const intpair pt)
{
	auto it = gdata->_map->getItem(pt);
	if (it != nullptr && (it->canStack() || it->_category == ITEM_SPELLBOOK))
	{
		addToInventory(gdata, it);
		gdata->_map->removeItem(pt);
		messages::add(gdata, "Got " + it->getVerboseName() + "!", it->getColor());
	}
}

//	If there's an item under us, try picking it up.
void tryPickupItem(gamedataPtr gdata)
{
	auto it = gdata->_map->getItem(gdata->_player->_pos);
	if (it != nullptr)
	{
		gdata->_pickingUp = it;
		gdata->_state = STATE_PICKUP_ITEM;
	}
	else
		messages::add(gdata, "Nothing here to pick up!", COLOR_NEGATIVE);
}


//	Try to buy the item at the current index in the current shop
void tryBuyCurrentItem(gamedataPtr gdata)
{
	if (gdata->_idx >= 0 && gdata->_idx < gdata->_currentShop->_items.size())
	{
		//	can we afford it?
		if (gdata->_gold >= gdata->_currentShop->_costs[gdata->_idx])
		{
			//	get and pay for the item
			auto it = gdata->_currentShop->_items[gdata->_idx];
			messages::add(gdata, "Bought " + it->getName() + ".", it->getColor());
			addToInventory(gdata, it);
			gdata->_gold -= gdata->_currentShop->_costs[gdata->_idx];

			//	remove it from the shop
			gdata->_currentShop->_items.erase(gdata->_currentShop->_items.begin() + gdata->_idx);
			gdata->_currentShop->_costs.erase(gdata->_currentShop->_costs.begin() + gdata->_idx);
		}
		else
			messages::add(gdata, "Not enough gold!", COLOR_NEGATIVE);
	}
	
	//	deselect item
	gdata->_idx = -1;
}
