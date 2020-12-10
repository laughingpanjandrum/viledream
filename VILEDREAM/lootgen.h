#pragma once

#include "itemData.h"
#include "item.h"


namespace lootgen
{

	//	Item tables

	const vector<itemData::itid> WEAPONS_COMMON = {
		itemData::WEAPON_DAGGER, itemData::WEAPON_LONGSWORD, itemData::WEAPON_LONGSWORD, itemData::WEAPON_MACE,
		itemData::WEAPON_QUARTERSTAFF, itemData::WEAPON_SPEAR, itemData::WEAPON_WAR_AXE,
		itemData::RANGED_SHORTBOW,
	};

	const vector<itemData::itid> WEAPONS_RARE = {
		itemData::WEAPON_GREATAXE, itemData::WEAPON_GREATMAUL, itemData::WEAPON_GREATSWORD, itemData::WEAPON_WAR_PIKE,
		itemData::WEAPON_RAPIER, itemData::WEAPON_WHIP,
		itemData::RANGED_LONGBOW, itemData::RANGED_COMPOSITE_BOW,
	};

	const vector<itemData::itid> ARMOUR_COMMON = {
		itemData::ARMOUR_ROBES, itemData::ARMOUR_LEATHER, itemData::ARMOUR_SCALE,
		itemData::SHIELD_BUCKLER, itemData::SHIELD_KITE,
	};

	const vector<itemData::itid> ARMOUR_RARE = {
		itemData::ARMOUR_CHAIN, itemData::ARMOUR_PLATE,
		itemData::SHIELD_TOWER,
	};

	const vector<itemData::itid> RINGS_ALL = {
		itemData::RING_ARCANE_THIRST, itemData::RING_BLOOD_HUNGER, itemData::RING_CHILL_AURA, itemData::RING_DAMNATION, itemData::RING_DEFIANCE, 
		itemData::RING_HEALER, itemData::RING_LIFE, itemData::RING_MIGHTY_BLOWS, itemData::RING_RAMPAGE,
	};

	const vector<itemData::itid> COATINGS_ALL = {
		itemData::COATING_FLAME, itemData::COATING_ICE, itemData::COATING_LIGHTNING, itemData::COATING_POISON,
	};


	Enchantment rollCloakEnchant(const int dl);
	ItemQuality rollQualityTier(const int dl);


	itemData::itid rollCoatingId();
	itemData::itid rollConsumableId();
	itemData::itid rollThrownId();


	itemPtr rollAlchemyWeaponDrop(const int dl);
	itemPtr rollAmmoDrop(const int dl, bool forceSpecial = false);
	itemPtr rollArmourDrop(const int dl);
	itemPtr rollCloakDrop(const int dl);
	itemPtr rollConsumableDrop(const int dl);
	itemPtr rollPotionDrop(const int dl);
	itemPtr rollRingDrop(const int dl);
	itemPtr rollScrollDrop(const int dl);
	itemPtr rollSpellbookDrop(const int dl);
	itemPtr rollWeaponDrop(const int dl);



	//	Rolling items

	itemData::itid rollIdOnTable(const vector<itemData::itid>* table);
	vector<itemPtr> rollDropTable(const int dl);


	//	Rolling shops

	itemShopPtr generate_AlchemyShop();
	itemShopPtr generate_AmmoStore();
	itemShopPtr generate_PotionShop();
	itemShopPtr generate_SpellShop();


}