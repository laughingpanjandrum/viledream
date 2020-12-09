#pragma once

#include <string>
#include <vector>
#include "color_constants.h"
#include "enums.h"

using namespace std;


enum WeaponTag {
	
	//	standard weapon attacks
	WTAG_CLEAVE, WTAG_CRITICAL, WTAG_DOUBLE, WTAG_LUNGE, WTAG_REACH, WTAG_RIPOSTE, WTAG_STAGGER, WTAG_WHIP,

	//	special attacks, monster-only
	WTAG_DROWNING,

	WTAG__NONE, };

string getWeaponTagName(const WeaponTag wtag);


namespace itemData
{

	enum itid {

		ARMOUR_ROBES, ARMOUR_LEATHER, ARMOUR_SCALE,
		ARMOUR_CHAIN, ARMOUR_PLATE,
		ARMOUR_CLOAK,

		ARROWS_STANDARD,
		ARROWS_POISON, ARROWS_SERRATED, ARROWS_SILVERED,

		COATING_FLAME, COATING_ICE, COATING_LIGHTNING, COATING_POISON,

		GOLD,

		POTION_HASTE, POTION_MIGHT, POTION_STONESKIN,

		RANGED_SHORTBOW, RANGED_LONGBOW, RANGED_COMPOSITE_BOW,

		RING_ARCANE_THIRST, RING_BLOOD_HUNGER, RING_CHILL_AURA, RING_DAMNATION, RING_DEFIANCE, RING_HEALER, RING_LIFE, RING_MIGHTY_BLOWS, RING_RAMPAGE,

		SCROLL_PORTAL,

		SHIELD_BUCKLER, SHIELD_KITE, SHIELD_TOWER,

		SPELLBOOK_NORMAL,

		URN_FIRE, URN_ICE, URN_LIGHTNING,
		URN_OIL, URN_WATER,

		WEAPON_DAGGER, WEAPON_RAPIER, WEAPON_QUARTERSTAFF, WEAPON_WHIP,
		WEAPON_LONGSWORD, WEAPON_GREATSWORD, WEAPON_SPEAR, WEAPON_WAR_PIKE,
		WEAPON_MACE, WEAPON_GREATMAUL, WEAPON_WAR_AXE, WEAPON_GREATAXE,

		ITEM__ID__NONE,
	};


	ItemCategory getDefaultCategory(const itid id);


	//	Flavour getters
	const string getName(const itid id);
	const int getGlyph(const itid id);
	const colorType getColor(const itid id);
	const string getDescription(const itid id);


	//	Weapon data
	int getAccuracyModifier(const itid id);
	intpair getDamageDice(const itid id);
	int getAttackRange(const itid id);
	WeaponTag getWeaponTag(const itid id);


	//	Armour/shield data
	int getDamageResist(const itid id);
	int getEvasionAdjust(const itid id);
	int getBlockValue(const itid id);

	
	//	Requirements (STR for weapons/shields, CON for armour)
	int getStatRequirement(const itid id);


}