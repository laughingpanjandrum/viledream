#pragma once

#include <string>
#include <vector>
#include "color_constants.h"

using namespace std;



//	Player vital attributes

enum Attribute { ATTR_STRENGTH, ATTR_DEXTERITY, ATTR_AGILITY, ATTR_CONSTITUTION, ATTR_CHARISMA, ATTR__MAX, };

const string getAttributeName(const Attribute attr);
const string getAttributeAbbr(const Attribute attr);
const string getAttributeDescription(const Attribute attr);


//	Damage types

enum DamageType { DTYPE_PHYSICAL, DTYPE_ADAMANT, DTYPE_COLD, DTYPE_ELECTRIC, DTYPE_FIRE, DTYPE_ETHEREAL, DTYPE_NECROTIC, DTYPE__MAX, };

const vector<DamageType> SPECIAL_DAMAGE_TYPES = { DTYPE_COLD, DTYPE_ELECTRIC, DTYPE_FIRE, DTYPE_ETHEREAL, DTYPE_NECROTIC, };

const string getDamageTypeName(const DamageType dt);
const colorType getDamageTypeColor(const DamageType dt);


//	Afflictions

enum Affliction {
	AFFL_AGONY, AFFL_BLEED, AFFL_CURSED, AFFL_FEAR, AFFL_LIMNED, AFFL_NAUSEA, AFFL_PETRIFY, AFFL_POISON, AFFL_STAGGER,
	AFFL_BURN, AFFL_DRAIN, AFFL_ELECTRIFY, AFFL_FREEZE,
	AFFL__NONE,
};

const string getAfflictionName(const Affliction af);
const colorType getAfflictionColor(const Affliction af);


//	Buffs

enum BuffType {

	//	Coatings
	BUFF_COATING_FLAME, BUFF_COATING_FROST, BUFF_COATING_LIGHTNING, BUFF_COATING_POISON,

	//	Potions
	BUFF_HASTE, BUFF_MIGHT, BUFF_STONESKIN,

	//	Other
	BUFF_RAMPAGE,

	BUFF__NONE,
};

const string getBuffName(const BuffType bf);
const colorType getBuffColor(const BuffType bf);


//	Conditions

enum Condition {
	COND_CHILLED,
	COND_HEATED,
	COND_OILY,
	COND_SOAKED,
	COND__NONE,
};

const string getConditionName(const Condition cond);
const colorType getConditionColor(const Condition cond);


//	Resistances

enum ResistanceType
{
	RESIST_COLD, RESIST_ELECTRIC, RESIST_FIRE, RESIST_NECROTIC,
	RESIST_AGONY, RESIST_BLEED, RESIST_FEAR, RESIST_PETRIFY, RESIST_POISON, RESIST_STAGGER,
	RESIST__NONE,
};

const vector<ResistanceType> RESISTANCES_PRIMARY = { RESIST_COLD, RESIST_ELECTRIC, RESIST_FIRE, RESIST_NECROTIC };
const vector<ResistanceType> RESISTANCES_SECONDARY = { RESIST_AGONY, RESIST_BLEED, RESIST_FEAR, RESIST_PETRIFY, RESIST_POISON, RESIST_STAGGER };

const string getResistanceName(const ResistanceType res);
const colorType getResistanceColor(const ResistanceType res);

ResistanceType getResistanceForDamageType(const DamageType dt);
ResistanceType getResistanceForAffliction(const Affliction af);


//	Item categories

enum ItemCategory { ITEM_AMMO, ITEM_ARMOUR, ITEM_CLOAK, ITEM_CONSUMABLE, ITEM_GOLD, ITEM_RANGED, 
	ITEM_RING, ITEM_SHIELD, ITEM_SPELLBOOK, ITEM_THROWN, ITEM_WEAPON, ITEM_MISC, };

const vector<ItemCategory> CONSUMABLE_CATEGORIES = { ITEM_AMMO, ITEM_CONSUMABLE, ITEM_THROWN, };

const string getItemCategoryName(const ItemCategory cat);


//	Item enchants

enum Enchantment { 

	//	cloak enchantments
	ENCH_BLADES, ENCH_DEFLECTION, ENCH_EVASION, ENCH_RESISTANCE, 

	ENCH__NONE,
};

const string getEnchantmentName(const Enchantment ench);


//	Item qualities

enum ItemQuality { ITEM_QUALITY_REFINED, ITEM_QUALITY_SUPERB, ITEM_QUALITY_MASTERWORK, ITEM_QUALITY_NORMAL, };

string getItemQualityName(const ItemQuality q);