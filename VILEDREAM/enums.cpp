#include "enums.h"

const string getAttributeName(const Attribute attr)
{
	switch (attr)
	{
	case(ATTR_AGILITY):			return "Agility";
	case(ATTR_CHARISMA):		return "Charisma";
	case(ATTR_CONSTITUTION):	return "Constitution";
	case(ATTR_DEXTERITY):		return "Dexterity";
	case(ATTR_STRENGTH):		return "Strength";
	default:
		return "attribute " + to_string(attr);
	}
}

const string getAttributeAbbr(const Attribute attr)
{
	switch (attr)
	{
	case(ATTR_AGILITY):			return "AGI";
	case(ATTR_CHARISMA):		return "CHA";
	case(ATTR_CONSTITUTION):	return "CON";
	case(ATTR_DEXTERITY):		return "DEX";
	case(ATTR_STRENGTH):		return "STR";
	default:
		return "attribute " + to_string(attr);
	}
}

const string getAttributeDescription(const Attribute attr)
{
	switch (attr)
	{
	case(ATTR_AGILITY):			return "Increases your chance to evade attacks.";
	case(ATTR_CHARISMA):		return "Increases maximum magic points.";
	case(ATTR_CONSTITUTION):	return "Increases maximum hit points and resistances.";
	case(ATTR_DEXTERITY):		return "Increases accuracy with melee and ranged attacks.";
	case(ATTR_STRENGTH):		return "Increases damage with melee attacks and some ranged attacks.";
	default:
		return "attribute " + to_string(attr);
	}
}

const string getDamageTypeName(const DamageType dt)
{
	switch (dt)
	{
	case(DTYPE_ADAMANT):	return "adamant";
	case(DTYPE_COLD):		return "cold";
	case(DTYPE_ELECTRIC):	return "electric";
	case(DTYPE_ETHEREAL):	return "ethereal";
	case(DTYPE_FIRE):		return "fire";
	case(DTYPE_NECROTIC):	return "necrotic";
	case(DTYPE_PHYSICAL):	return "physical";
	default:
		return "damage type " + to_string(dt);
	}
}

const colorType getDamageTypeColor(const DamageType dt)
{
	switch (dt)
	{
	case(DTYPE_ADAMANT):	return TCODColor::lightChartreuse;
	case(DTYPE_COLD):		return TCODColor::cyan;
	case(DTYPE_ELECTRIC):	return TCODColor::yellow;
	case(DTYPE_ETHEREAL):	return TCODColor::lightPurple;
	case(DTYPE_FIRE):		return TCODColor::flame;
	case(DTYPE_NECROTIC):	return TCODColor::sepia;
	case(DTYPE_PHYSICAL):	return TCODColor::lightGrey;
	default:
		return COLOR_WHITE;
	}
}

const string getAfflictionName(const Affliction af)
{
	switch (af)
	{
	case(AFFL_AGONY):		return "Agony";
	case(AFFL_BLEED):		return "Bleed";
	case(AFFL_BURN):		return "Burn";
	case(AFFL_CURSED):		return "Cursed";
	case(AFFL_DRAIN):		return "Drain";
	case(AFFL_ELECTRIFY):	return "Electrify";
	case(AFFL_FEAR):		return "Fear";
	case(AFFL_FREEZE):		return "Freeze";
	case(AFFL_LIMNED):		return "Limned";
	case(AFFL_NAUSEA):		return "Nausea";
	case(AFFL_PETRIFY):		return "Petrify";
	case(AFFL_POISON):		return "Poison";
	case(AFFL_STAGGER):		return "Stagger";
	default:
		return "affliction " + to_string(af);	
	}
}

const colorType getAfflictionColor(const Affliction af)
{
	switch (af)
	{
	case(AFFL_AGONY):		return TCODColor::lightOrange;
	case(AFFL_BLEED):		return TCODColor::crimson;
	case(AFFL_BURN):		return TCODColor::flame;
	case(AFFL_CURSED):		return TCODColor::darkRed;
	case(AFFL_DRAIN):		return TCODColor::lightPurple;
	case(AFFL_ELECTRIFY):	return TCODColor::yellow;
	case(AFFL_FEAR):		return TCODColor::orange;
	case(AFFL_FREEZE):		return TCODColor::cyan;
	case(AFFL_LIMNED):		return TCODColor::silver;
	case(AFFL_NAUSEA):		return TCODColor::sepia;
	case(AFFL_PETRIFY):		return TCODColor::grey;
	case(AFFL_POISON):		return TCODColor::lime;
	case(AFFL_STAGGER):		return TCODColor::lightBlue;
	default:
		return COLOR_WHITE;
	}
}

const string getBuffName(const BuffType bf)
{
	switch (bf)
	{
	case(BUFF_COATING_FLAME):		return "Flame Coating";
	case(BUFF_COATING_FROST):		return "Frost Coating";
	case(BUFF_COATING_LIGHTNING):	return "Lightning Coating";
	case(BUFF_COATING_POISON):		return "Poison Coating";
	case(BUFF_HASTE):				return "Haste";
	case(BUFF_MIGHT):				return "Might";
	case(BUFF_RAMPAGE):				return "Rampage";
	case(BUFF_STONESKIN):			return "Stoneskin";
	default:
		return "buff " + to_string(bf);
	}
}

const colorType getBuffColor(const BuffType bf)
{
	switch (bf)
	{
	case(BUFF_COATING_FLAME):		return getDamageTypeColor(DTYPE_FIRE);
	case(BUFF_COATING_FROST):		return getDamageTypeColor(DTYPE_COLD);
	case(BUFF_COATING_LIGHTNING):	return getDamageTypeColor(DTYPE_ELECTRIC);
	case(BUFF_COATING_POISON):		return getAfflictionColor(AFFL_POISON);
	
	case(BUFF_HASTE):				return TCODColor::lighterBlue;
	case(BUFF_MIGHT):				return TCODColor::orange;
	case(BUFF_STONESKIN):			return TCODColor::lightGrey;

	case(BUFF_RAMPAGE):				return TCODColor::crimson;
	default:
		return COLOR_WHITE;
	}
}

const string getConditionName(const Condition cond)
{
	switch (cond)
	{
	case(COND_CHILLED):		return "Chilled";
	case(COND_HEATED):		return "Heated";
	case(COND_OILY):		return "Oily";
	case(COND_SOAKED):		return "Soaked";
	default:
		return "condition " + to_string(cond);
	}
}

const colorType getConditionColor(const Condition cond)
{
	switch (cond)
	{
	case(COND_CHILLED):		return TCODColor::cyan;
	case(COND_HEATED):		return TCODColor::darkFlame;
	case(COND_OILY):		return TCODColor::purple;
	case(COND_SOAKED):		return TCODColor::lightBlue;
	default:
		return COLOR_WHITE;
	}
}

const string getResistanceName(const ResistanceType res)
{
	switch (res)
	{
	case(RESIST_AGONY):		return "Agony";
	case(RESIST_BLEED):		return "Bleed";
	case(RESIST_COLD):		return "Cold";
	case(RESIST_ELECTRIC):	return "Electric";
	case(RESIST_FEAR):		return "Fear";
	case(RESIST_FIRE):		return "Fire";
	case(RESIST_NECROTIC):	return "Necrotic";
	case(RESIST_PETRIFY):	return "Petrify";
	case(RESIST_POISON):	return "Poison";
	case(RESIST_STAGGER):	return "Stagger";
	default:
		return "resistance " + to_string(res);
	}
}

const colorType getResistanceColor(const ResistanceType res)
{
	switch (res)
	{
	case(RESIST_AGONY):		return getAfflictionColor(AFFL_AGONY);
	case(RESIST_BLEED):		return getAfflictionColor(AFFL_BLEED);
	case(RESIST_COLD):		return getDamageTypeColor(DTYPE_COLD);
	case(RESIST_ELECTRIC):	return getDamageTypeColor(DTYPE_ELECTRIC);
	case(RESIST_FEAR):		return getAfflictionColor(AFFL_FEAR);
	case(RESIST_FIRE):		return getDamageTypeColor(DTYPE_FIRE);
	case(RESIST_NECROTIC):	return getDamageTypeColor(DTYPE_NECROTIC);
	case(RESIST_PETRIFY):	return getAfflictionColor(AFFL_PETRIFY);
	case(RESIST_POISON):	return getAfflictionColor(AFFL_POISON);
	case(RESIST_STAGGER):	return getAfflictionColor(AFFL_STAGGER);
	default:
		return COLOR_WHITE;
	}
}

ResistanceType getResistanceForDamageType(const DamageType dt)
{
	switch (dt)
	{
	case(DTYPE_COLD):		return RESIST_COLD;
	case(DTYPE_ELECTRIC):	return RESIST_ELECTRIC;
	case(DTYPE_FIRE):		return RESIST_FIRE;
	case(DTYPE_NECROTIC):	return RESIST_NECROTIC;
	default:
		return RESIST__NONE;
	}
}

ResistanceType getResistanceForAffliction(const Affliction af)
{
	switch (af)
	{
	case(AFFL_AGONY):		return RESIST_AGONY;
	case(AFFL_BLEED):		return RESIST_BLEED;	
	case(AFFL_FEAR):		return RESIST_FEAR;
	case(AFFL_PETRIFY):		return RESIST_PETRIFY;
	case(AFFL_POISON):		return RESIST_POISON;
	case(AFFL_STAGGER):		return RESIST_STAGGER;

	case(AFFL_BURN):		return RESIST_FIRE;
	case(AFFL_ELECTRIFY):	return RESIST_ELECTRIC;
	case(AFFL_DRAIN):		return RESIST_NECROTIC;
	case(AFFL_FREEZE):		return RESIST_COLD;

	default:
		return RESIST__NONE;
	}
}

const string getItemCategoryName(const ItemCategory cat)
{
	switch (cat)
	{
	case(ITEM_AMMO):		return "Ammo";
	case(ITEM_ARMOUR):		return "Armour";
	case(ITEM_CLOAK):		return "Cloak";
	case(ITEM_CONSUMABLE):	return "Consumable";
	case(ITEM_GOLD):		return "Gold";
	case(ITEM_RANGED):		return "Ranged Weapon";
	case(ITEM_RING):		return "Ring";
	case(ITEM_SHIELD):		return "Shield";
	case(ITEM_SPELLBOOK):	return "Spellbook";
	case(ITEM_THROWN):		return "Thrown Weapon";
	case(ITEM_WEAPON):		return "Melee Weapon";
	default:
		return "item category " + to_string(cat);
	}
}

const string getEnchantmentName(const Enchantment ench)
{
	switch (ench)
	{
	case(ENCH_BLADES):			return "blades";
	case(ENCH_DEFLECTION):		return "deflection";
	case(ENCH_EVASION):			return "evasion";
	case(ENCH_RESISTANCE):		return "resistance";

	default:
		return "enchantment " + to_string(ench);
	}
}

string getItemQualityName(const ItemQuality q)
{
	switch (q)
	{
	case(ITEM_QUALITY_MASTERWORK):		return "masterwork";
	case(ITEM_QUALITY_REFINED):			return "refined";
	case(ITEM_QUALITY_SUPERB):			return "superb";
	default:
		return "";
	}
}
