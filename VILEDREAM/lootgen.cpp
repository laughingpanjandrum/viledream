#include "lootgen.h"



//	Enchantment type for a cloak.
Enchantment lootgen::rollCloakEnchant(const int dl)
{
	int r = randint(1, 4);
	if		(r == 1)	return ENCH_BLADES;
	else if	(r == 2)	return ENCH_DEFLECTION;
	else if (r == 3)	return ENCH_EVASION;
	else if (r == 4)	return ENCH_RESISTANCE;
	return ENCH__NONE;
}


//	Random quality tier for any item. Higher DL makes better quality more likely.
ItemQuality lootgen::rollQualityTier(const int dl)
{
	int r = randint(1, 200 - dl * 5);
	if		(r <= 3 && dl > 10)	return ITEM_QUALITY_MASTERWORK;
	else if (r <= 10 && dl > 5)	return ITEM_QUALITY_SUPERB;
	else if (r <= 25 && dl > 2)	return ITEM_QUALITY_REFINED;
	return ITEM_QUALITY_NORMAL;
}


itemData::itid lootgen::rollCoatingId()
{
	return rollIdOnTable(&COATINGS_ALL);
}

//	Roll a random consumable. Each has its own weight.
itemData::itid lootgen::rollConsumableId()
{
	int r = randint(1, 16);

	//	potions
	if		(r <= 1)	return itemData::POTION_HASTE;
	else if (r <= 4)	return itemData::POTION_MIGHT;
	else if (r <= 7)	return itemData::POTION_STONESKIN;

	//	coatings
	else if (r <= 9)	return itemData::COATING_FLAME;
	else if (r <= 11)	return itemData::COATING_ICE;
	else if (r <= 13)	return itemData::COATING_LIGHTNING;
	else if (r <= 15)	return itemData::COATING_POISON;

	//	scrolls
	else if (r <= 16)	return itemData::SCROLL_PORTAL;

	//	we should never fall through!! but you know, just in case
	return itemData::URN_WATER;
}


//	Random thrown weapon.
itemData::itid lootgen::rollThrownId()
{
	int r = randint(1, 16);

	if		(r <= 4)	return itemData::URN_FIRE;
	else if (r <= 8)	return itemData::URN_ICE;
	else if (r <= 12)	return itemData::URN_LIGHTNING;
	else if (r <= 14)	return itemData::URN_OIL;
	else if (r <= 16)	return itemData::URN_WATER;

	//	we should never fall through!! but you know, just in case
	return itemData::URN_WATER;
}


//	Any alchemical weapon (thrown items, coatings)
itemPtr lootgen::rollAlchemyWeaponDrop(const int dl)
{
	if (roll_one_in(4))
		return itemPtr(new item(rollCoatingId()));
	else
		return itemPtr(new item(rollThrownId()));
}



//	Stack of arrows.
//	If 'forceSpecial' is set, will not generate standard arrows.
itemPtr lootgen::rollAmmoDrop(const int dl, bool forceSpecial)
{
	//	choose type
	itemData::itid id = itemData::ARROWS_STANDARD;
	int r = randint(1, forceSpecial ? 25 : 1);
	if		(r <= 10)	id = itemData::ARROWS_POISON;
	else if (r <= 20)	id = itemData::ARROWS_SERRATED;
	else if (r <= 25)	id = itemData::ARROWS_SILVERED;
	
	//	choose amount
	int amt = dieRoll(2, 6);
	return itemPtr(new item(id, amt));
}



//	A random armour piece.
itemPtr lootgen::rollArmourDrop(const int dl)
{
	//	determine armour rarity
	itemData::itid id;
	if (roll_one_in(4))
		id = rollIdOnTable(&ARMOUR_RARE);
	else
		id = rollIdOnTable(&ARMOUR_COMMON);
	auto it = itemPtr(new item(id));

	//	roll quality
	it->setQuality(rollQualityTier(dl));

	return it;
}


//	A random cloak. Better enchantment bonuses more common at higher tiers.
itemPtr lootgen::rollCloakDrop(const int dl)
{
	auto it = itemPtr(new item(itemData::ARMOUR_CLOAK));

	//	roll enchantment type
	auto ench = rollCloakEnchant(dl);

	//	roll enchantment level
	int roll = dieRoll(2, 8) - 11;
	int bns = 1 + MAX(0, roll);

	it->setEnchantment(ench, bns);
	return it;
}



//	A random consumable.
itemPtr lootgen::rollConsumableDrop(const int dl)
{
	itemPtr it = nullptr;
	int r = randint(1, 100);
	if (r <= 50)
		it = rollPotionDrop(dl);
	else if (r <= 90)
		it = rollAlchemyWeaponDrop(dl);
	else
		it = rollScrollDrop(dl);
	
	//	chance to drop more than 1
	int amt = 1;
	if (roll_one_in(20))
		amt += randint(1, 2);
	it->_amount = amt;
	return it;
}


//	A random potion.
itemPtr lootgen::rollPotionDrop(const int dl)
{
	itemData::itid id = itemData::ITEM__ID__NONE;
	int r = randint(1, 100);
	if		(r <= 30)	id = itemData::POTION_HASTE;
	else if (r <= 65)	id = itemData::POTION_MIGHT;
	else if (r <= 100)	id = itemData::POTION_STONESKIN;
	return itemPtr(new item(id));
}



//	A random ring.
itemPtr lootgen::rollRingDrop(const int dl)
{
	auto id = rollIdOnTable(&RINGS_ALL);
	return itemPtr(new item(id));
}


//	A random scroll.
itemPtr lootgen::rollScrollDrop(const int dl)
{
	return itemPtr(new item(itemData::SCROLL_PORTAL));
}



//	A random spellbook. Higher-level spells are more common at higher DL's.
itemPtr lootgen::rollSpellbookDrop(const int dl)
{
	int i = randrange(SPELL__NONE);
	auto sp = static_cast<Spell>(i);
	return itemPtr(new item(sp));
}



//	Random weapon drop. Better weapons more likely at higher DL's.
itemPtr lootgen::rollWeaponDrop(const int dl)
{
	//	determine weapon rarity
	itemData::itid id;
	if (roll_one_in(4))
		id = rollIdOnTable(&WEAPONS_RARE);
	else
		id = rollIdOnTable(&WEAPONS_COMMON);
	auto it = itemPtr(new item(id));

	//	roll quality
	it->setQuality(rollQualityTier(dl));

	return it;
}


//	Return a random element from the given list, equally weighted
itemData::itid lootgen::rollIdOnTable(const vector<itemData::itid>* table)
{
	return table->at(randrange(table->size()));
}



//	List of loot drops for a given danger level
vector<itemPtr> lootgen::rollDropTable(const int dl)
{
	//	items
	vector<itemPtr> drops;
	int amt = 4 + dieRoll(6, 3);
	while (amt-- > 0)
	{
		//	roll type of item
		int r = randint(1, 100);
		if		(r <= 20)	drops.push_back(rollWeaponDrop(dl));
		else if (r <= 30)	drops.push_back(rollArmourDrop(dl));
		else if (r <= 80)	drops.push_back(rollConsumableDrop(dl));
		else if (r <= 95)	drops.push_back(rollAmmoDrop(dl));
		else if (r <= 97)	drops.push_back(rollSpellbookDrop(dl));
		else if (r <= 98)	drops.push_back(rollRingDrop(dl));
		else if (r <= 100)	drops.push_back(rollCloakDrop(dl));
	}


	//	money
	amt = dieRoll(4, 2);
	while (amt-- > 0)
	{
		int val = dieRoll(dl, 6);
		drops.push_back(itemPtr(new item(itemData::GOLD, val)));
	}


	return drops;
}

itemShopPtr lootgen::generate_AlchemyShop()
{
	auto shop = itemShopPtr(new itemShop());
	int icount = 3 + dieRoll(2, 6);
	while (icount-- > 0)
	{
		auto it = rollAlchemyWeaponDrop(20);
		it->_amount = randint(1, 3);
		shop->_items.push_back(it);
		shop->_costs.push_back(it->getGoldCost());
	}
	return shop;
}

itemShopPtr lootgen::generate_AmmoStore()
{
	auto shop = itemShopPtr(new itemShop());
	
	//	standard arrows
	for (unsigned i = 1; i <= 5; i++)
	{
		auto it = itemPtr(new item(itemData::ARROWS_STANDARD, 10 * i));
		shop->_items.push_back(it);
		shop->_costs.push_back(it->getGoldCost());
	}

	// special arrows
	int icount = dieRoll(4, 2);
	while (icount-- > 0)
	{
		auto it = rollAmmoDrop(20, true);
		shop->_items.push_back(it);
		shop->_costs.push_back(it->getGoldCost());
	}
	return shop;
}

itemShopPtr lootgen::generate_PotionShop()
{
	auto shop = itemShopPtr(new itemShop());
	int icount = 3 + dieRoll(2, 6);
	while (icount-- > 0)
	{
		auto it = rollPotionDrop(20);
		it->_amount = randint(1, 6);
		shop->_items.push_back(it);
		shop->_costs.push_back(it->getGoldCost());
	}
	return shop;
}

itemShopPtr lootgen::generate_SpellShop()
{
	auto shop = itemShopPtr(new itemShop());
	int icount = dieRoll(3, 4);
	while (icount-- > 0)
	{
		auto sp = rollSpellbookDrop(randint(1, 20));
		shop->_items.push_back(sp);
		shop->_costs.push_back(sp->getGoldCost());
	}
	return shop;
}
