#include "itemData.h"


ItemCategory itemData::getDefaultCategory(const itid id)
{
	switch (id)
	{
	case(ARMOUR_CHAIN):
	case(ARMOUR_LEATHER):
	case(ARMOUR_PLATE):
	case(ARMOUR_ROBES):
	case(ARMOUR_SCALE):
		return ITEM_ARMOUR;

	case(ARROWS_STANDARD):
	case(ARROWS_POISON):
	case(ARROWS_SERRATED):
	case(ARROWS_SILVERED):
		return ITEM_AMMO;

	case(ARMOUR_CLOAK):
		return ITEM_CLOAK;

	case(GOLD):	
		return ITEM_GOLD;

	case(COATING_FLAME):
	case(COATING_ICE):
	case(COATING_LIGHTNING):
	case(COATING_POISON):
	case(POTION_HASTE):
	case(POTION_MIGHT):
	case(POTION_STONESKIN):
	case(SCROLL_PORTAL):
		return ITEM_CONSUMABLE;

	case(RANGED_COMPOSITE_BOW):
	case(RANGED_LONGBOW):
	case(RANGED_SHORTBOW):
		return ITEM_RANGED;

	case(RING_ARCANE_THIRST):
	case(RING_BLOOD_HUNGER):
	case(RING_CHILL_AURA):
	case(RING_DAMNATION):
	case(RING_DEFIANCE):
	case(RING_HEALER):
	case(RING_LIFE):
	case(RING_MIGHTY_BLOWS):
	case(RING_RAMPAGE):
		return ITEM_RING;

	case(SHIELD_BUCKLER):
	case(SHIELD_KITE):
	case(SHIELD_TOWER):
		return ITEM_SHIELD;

	case(SPELLBOOK_NORMAL):
		return ITEM_SPELLBOOK;
		
	case(URN_FIRE):
	case(URN_ICE):
	case(URN_LIGHTNING):
	case(URN_OIL):
	case(URN_WATER):
		return ITEM_THROWN;

	case(WEAPON_DAGGER):
	case(WEAPON_GREATAXE):
	case(WEAPON_GREATMAUL):
	case(WEAPON_GREATSWORD):
	case(WEAPON_LONGSWORD):
	case(WEAPON_MACE):
	case(WEAPON_QUARTERSTAFF):
	case(WEAPON_RAPIER):
	case(WEAPON_SPEAR):
	case(WEAPON_WAR_AXE):
	case(WEAPON_WAR_PIKE):
	case(WEAPON_WHIP):
		return ITEM_WEAPON;

	default:
		return ITEM_MISC;
	}
}

const string itemData::getName(const itid id)
{
	switch (id)
	{
	case(ARROWS_STANDARD):			return "arrows";
	case(ARROWS_POISON):			return "poison arrows";
	case(ARROWS_SERRATED):			return "serrated arrows";
	case(ARROWS_SILVERED):			return "silvered arrows";

	case(ARMOUR_CHAIN):				return "chainmail armour";
	case(ARMOUR_LEATHER):			return "leather armour";	
	case(ARMOUR_PLATE):				return "plate armour";
	case(ARMOUR_ROBES):				return "robe";
	case(ARMOUR_SCALE):				return "scale armour";

	case(ARMOUR_CLOAK):				return "cloak";

	case(COATING_FLAME):			return "flame coating";
	case(COATING_ICE):				return "frost coating";
	case(COATING_LIGHTNING):		return "lightning coating";
	case(COATING_POISON):			return "poison coating";

	case(GOLD):						return "gold";

	case(POTION_HASTE):				return "potion of haste";
	case(POTION_MIGHT):				return "potion of might";
	case(POTION_STONESKIN):			return "potion of stoneskin";

	case(RANGED_COMPOSITE_BOW):		return "composite longbow";
	case(RANGED_LONGBOW):			return "longbow";
	case(RANGED_SHORTBOW):			return "shortbow";

	case(RING_ARCANE_THIRST):		return "ring of arcane thirst";
	case(RING_BLOOD_HUNGER):		return "ring of blood hunger";
	case(RING_CHILL_AURA):			return "ring of chill aura";
	case(RING_DAMNATION):			return "ring of damnation";
	case(RING_DEFIANCE):			return "ring of defiance";
	case(RING_HEALER):				return "ring of the healer";
	case(RING_LIFE):				return "ring of life";
	case(RING_MIGHTY_BLOWS):		return "ring of mighty blows";
	case(RING_RAMPAGE):				return "ring of rampage";

	case(SCROLL_PORTAL):			return "portal scroll";

	case(SHIELD_BUCKLER):			return "buckler";
	case(SHIELD_KITE):				return "kite shield";	
	case(SHIELD_TOWER):				return "tower shield";
		
	case(URN_FIRE):					return "urn of fire";
	case(URN_ICE):					return "urn of ice";
	case(URN_LIGHTNING):			return "urn of lightning";
	case(URN_OIL):					return "urn of oil";
	case(URN_WATER):				return "urn of water";

	case(WEAPON_DAGGER):			return "dagger";
	case(WEAPON_GREATAXE):			return "greataxe";
	case(WEAPON_GREATMAUL):			return "greatmaul";
	case(WEAPON_GREATSWORD):		return "greatsword";
	case(WEAPON_LONGSWORD):			return "longsword";
	case(WEAPON_MACE):				return "mace";
	case(WEAPON_QUARTERSTAFF):		return "quarterstaff";
	case(WEAPON_RAPIER):			return "rapier";
	case(WEAPON_SPEAR):				return "spear";
	case(WEAPON_WAR_AXE):			return "war axe";
	case(WEAPON_WAR_PIKE):			return "war pike";
	case(WEAPON_WHIP):				return "whip";

	default:
		return "item " + to_string(id);
	}
}

const int itemData::getGlyph(const itid id)
{
	//	by id
	switch (id)
	{
	case(COATING_FLAME):
	case(COATING_ICE):
	case(COATING_LIGHTNING):
	case(COATING_POISON):
		return 229;

	case(POTION_HASTE):
	case(POTION_MIGHT):
	case(POTION_STONESKIN):
		return '!';

	case(SCROLL_PORTAL):
		return 13;

	case(URN_FIRE):
	case(URN_ICE):
	case(URN_LIGHTNING):
	case(URN_OIL):
	case(URN_WATER):
		return 235;
	}

	//	by category
	switch (getDefaultCategory(id))
	{
	case(ITEM_AMMO):		return '|';
	case(ITEM_ARMOUR):		return '[';
	case(ITEM_CLOAK):		return '(';
	case(ITEM_GOLD):		return '$';
	case(ITEM_RANGED):		return '}';
	case(ITEM_RING):		return 248;
	case(ITEM_SHIELD):		return 4;
	case(ITEM_SPELLBOOK):	return 14;
	case(ITEM_WEAPON):		return '/';
	default:
		return '?';
	}
}

const colorType itemData::getColor(const itid id)
{
	switch (id)
	{

		//	Consumables

	case(COATING_FLAME):		return getDamageTypeColor(DTYPE_FIRE);
	case(COATING_ICE):			return getDamageTypeColor(DTYPE_COLD);
	case(COATING_LIGHTNING):	return getDamageTypeColor(DTYPE_ELECTRIC);
	case(COATING_POISON):		return getAfflictionColor(AFFL_POISON);

	case(POTION_HASTE):			return getBuffColor(BUFF_HASTE);
	case(POTION_MIGHT):			return getBuffColor(BUFF_MIGHT);
	case(POTION_STONESKIN):		return getBuffColor(BUFF_STONESKIN);

	case(SCROLL_PORTAL):		return TCODColor::pink;


		//	Thrown weapons

	case(URN_FIRE):				return TCODColor::flame;
	case(URN_ICE):				return TCODColor::cyan;
	case(URN_LIGHTNING):		return TCODColor::yellow;
	case(URN_OIL):				return TCODColor::purple;
	case(URN_WATER):			return TCODColor::lightBlue;


		//	Rings

	case(RING_ARCANE_THIRST):
	case(RING_BLOOD_HUNGER):
	case(RING_CHILL_AURA):
	case(RING_DAMNATION):
	case(RING_DEFIANCE):
	case(RING_HEALER):
	case(RING_LIFE):
	case(RING_MIGHTY_BLOWS):
	case(RING_RAMPAGE):
		return TCODColor::gold;


		//	Ammo

	case(ARROWS_POISON):		return TCODColor::lime;
	case(ARROWS_SERRATED):		return TCODColor::crimson;
	case(ARROWS_SILVERED):		return TCODColor::silver;
	case(ARROWS_STANDARD):		return TCODColor::lightGrey;


		//	Other

	case(GOLD):		return TCODColor::gold;


		//	Equipment

	case(ARMOUR_SCALE):
	case(WEAPON_DAGGER):
	case(WEAPON_RAPIER):
		return TCODColor::lighterGrey;

	case(ARMOUR_CHAIN):
	case(SHIELD_KITE):
	case(WEAPON_LONGSWORD):
	case(WEAPON_MACE):
	case(WEAPON_WAR_AXE):
		return TCODColor::lightGrey;

	case(ARMOUR_PLATE):
	case(SHIELD_TOWER):
	case(WEAPON_GREATAXE):
	case(WEAPON_GREATMAUL):
	case(WEAPON_GREATSWORD):
		return TCODColor::grey;

	case(ARMOUR_CLOAK):
	case(RANGED_SHORTBOW):
	case(WEAPON_WHIP):
		return TCODColor::lighterSepia;

	case(RANGED_LONGBOW):
	case(SHIELD_BUCKLER):
	case(WEAPON_QUARTERSTAFF):
	case(WEAPON_SPEAR):
		return TCODColor::lightSepia;

	case(RANGED_COMPOSITE_BOW):
	case(ARMOUR_LEATHER):
	case(WEAPON_WAR_PIKE):
		return TCODColor::sepia;

	case(ARMOUR_ROBES):
		return TCODColor::red;
	
	default:
		return COLOR_WHITE;
	}
}

const string itemData::getDescription(const itid id)
{
	switch (id)
	{
	case(ARMOUR_CHAIN):				return "Armour composed of chain stitched to boiled leather. Excellent protection.";
	case(ARMOUR_LEATHER):			return "Armour of boiled leather. Hardly impedes movement, but provides little protection.";
	case(ARMOUR_PLATE):				return "Heavy armour made from connected plates. Unparalleled protection, but makes dodging difficult.";
	case(ARMOUR_ROBES):				return "A simple set of robes. Almost no protection, but it's better than nothing.";
	case(ARMOUR_SCALE):				return "Armour made from small scales stitched to boiled leather. Provides less protection than chain.";

	case(ARROWS_POISON):			return "Arrows tipped with poison. Slowly drain life on hit.";
	case(ARROWS_SERRATED):			return "Shafts tipped with serrated arrowsheads, designed to inflict bleeding.";
	case(ARROWS_SILVERED):			return "Arrows coated in alchemic silver, causing them to inflict extra damage to the undead.";
	case(ARROWS_STANDARD):			return "Feathered shafts with flint arrowheads. The most basic type of ammunition.";

	case(ARMOUR_CLOAK):				return "A cloak to wear over your armour.";

	case(COATING_FLAME):			return "Coat your weapon to add fire damage to your attacks for a while.";
	case(COATING_ICE):				return "Coat your weapon to add cold damage to your attacks for a while.";
	case(COATING_LIGHTNING):		return "Coat your weapon to add fire damage to your attacks for a while.";
	case(COATING_POISON):			return "Coat your weapon to add poison damage to your attacks for a while.";

	case(POTION_HASTE):				return "Quaff this potion to temporarily increase the speed of your attacks and movement.";
	case(POTION_MIGHT):				return "Quaff this potion to gain a temporary boost to damage in melee.";
	case(POTION_STONESKIN):			return "Quaff to strengthen your skin, granting extraordinary damage resistance but slowing your movements.";

	case(RANGED_COMPOSITE_BOW):		return "A longbow made from composite material. Unlike normal bows, it adds the damage bonus from Strength.";
	case(RANGED_LONGBOW):			return "A heavy longbow. Harder to draw than a shortbow, but much more rewarding.";
	case(RANGED_SHORTBOW):			return "A simple shortbow, easy to aim and fire.";

	case(RING_ARCANE_THIRST):		return "Your MP recharges slightly whenever you kill.";
	case(RING_BLOOD_HUNGER):		return "Healing surges are restored 10% faster on kills.";
	case(RING_CHILL_AURA):			return "When you are hit in melee, retaliate for 1d4 cold damage.";
	case(RING_DAMNATION):			return "Inflict +7 melee damage when your health is 30% or lower.";
	case(RING_DEFIANCE):			return "Get +5 Evasion when your health is 30% or lower.";
	case(RING_HEALER):				return "Surges heal 10% more health when used.";
	case(RING_LIFE):				return "Increases maximum healing surges by 1.";
	case(RING_MIGHTY_BLOWS):		return "Increases melee damage by +4, but adjusts melee accuracy by -2.";
	case(RING_RAMPAGE):				return "Kills grant a cumulative melee damage buff that wears off over time.";

	case(SCROLL_PORTAL):			return "Conjures a portal linking you to the safety of Fireholm.";

	case(SHIELD_BUCKLER):			return "A light shield. It's unlikely to block attacks, but it doesn't interfere with your movement.";
	case(SHIELD_KITE):				return "A standard kite shield. May block enemy attacks, but makes it harder to hit.";
	case(SHIELD_TOWER):				return "Enormous tower shield. Excellent at blocking attacks, but greatly impedes accuracy.";

	case(URN_FIRE):					return "Shatters when thrown, igniting an area and inflicting fire damage.";
	case(URN_ICE):					return "Shatters when thrown, freezing an area and inflicting cold damage.";
	case(URN_LIGHTNING):			return "Shatters when thrown, electrifying an area and inflicting electric damage.";
	case(URN_OIL):					return "Shatters when thrown, covering an area in oil.";
	case(URN_WATER):				return "Shatters when thrown, covering an area in pools of water.";

	case(WEAPON_DAGGER):			return "A short-bladed stabbing weapon. Deals extra damage on a critical hit.";
	case(WEAPON_GREATAXE):			return "A massive two-handed cleaving weapon. Strikes all adjacent enemies.";
	case(WEAPON_GREATMAUL):			return "A massive two-handed smashing weapon. May stagger enemies it hits.";
	case(WEAPON_GREATSWORD):		return "A two-handed slashing weapon. If you block or evade an attack, you get a free counter-attack.";
	case(WEAPON_LONGSWORD):			return "A one-handed slashing weapon. If you block or evade an attack, you get a free counter-attack.";
	case(WEAPON_MACE):				return "A simple one-handed smashing weapon. May stagger enemies it hits.";
	case(WEAPON_QUARTERSTAFF):		return "A simple wooden rod. Performs two attacks in a row.";
	case(WEAPON_RAPIER):			return "A lightweight piercing weapon. Move directly towards an enemy from 1 space away to auto-attack.";
	case(WEAPON_SPEAR):				return "A simple thrusting weapon. Press CTRL+[Direction] to attack at reach.";
	case(WEAPON_WAR_AXE):			return "A one-handed cleaving weapon. Attacks strike all adjacent enemies.";
	case(WEAPON_WAR_PIKE):			return "A two-handed thrusting weapon. Press CTRL+[Direction] to attack at reach.";
	case(WEAPON_WHIP):				return "A leather lash. Press CTRL+[Direction] to attack from up to 5 squares away.";

	default:
		return "";
	}
}

int itemData::getAccuracyModifier(const itid id)
{
	switch (id)
	{
	case(WEAPON_DAGGER):
		return 2;

	case(WEAPON_RAPIER):
		return 1;

	case(WEAPON_GREATSWORD):
	case(WEAPON_LONGSWORD):
	case(WEAPON_SPEAR):
	case(WEAPON_WAR_PIKE):
		return 0;

	case(RANGED_LONGBOW):
	case(SHIELD_KITE):
	case(WEAPON_GREATMAUL):
	case(WEAPON_MACE):
	case(WEAPON_WHIP):
		return -1;

	case(RANGED_COMPOSITE_BOW):
	case(WEAPON_GREATAXE):
	case(WEAPON_QUARTERSTAFF):
	case(WEAPON_WAR_AXE):
		return -2;

	case(SHIELD_TOWER):
		return -3;

	default:
		return 0;
	}
}

intpair itemData::getDamageDice(const itid id)
{
	switch (id)
	{
	case(SHIELD_BUCKLER):
	case(WEAPON_WHIP):
		return intpair(1, 3);

	case(WEAPON_DAGGER):
	case(WEAPON_QUARTERSTAFF):
	case(WEAPON_RAPIER):
		return intpair(1, 4);

	case(RANGED_SHORTBOW):
	case(SHIELD_KITE):
	case(WEAPON_SPEAR):
		return intpair(1, 6);

	case(RANGED_LONGBOW):
	case(RANGED_COMPOSITE_BOW):
	case(SHIELD_TOWER):
	case(WEAPON_LONGSWORD):
	case(WEAPON_WAR_AXE):
		return intpair(1, 8);

	case(WEAPON_WAR_PIKE):
		return intpair(1, 10);

	case(WEAPON_GREATAXE):
	case(WEAPON_GREATSWORD):
		return intpair(1, 12);
	
	case(WEAPON_MACE):
		return intpair(2, 4);

	case(WEAPON_GREATMAUL):
		return intpair(2, 6);
	
	default:
		return intpair(1, 1);
	}
}

int itemData::getAttackRange(const itid id)
{
	switch (id)
	{
	case(RANGED_SHORTBOW):			return 8;
	case(RANGED_LONGBOW):			return 12;
	case(RANGED_COMPOSITE_BOW):		return 16;
	default:
		return 1;
	}
}

WeaponTag itemData::getWeaponTag(const itid id)
{
	switch (id)
	{
	case(WEAPON_GREATAXE):
	case(WEAPON_WAR_AXE):
		return WTAG_CLEAVE;

	case(WEAPON_DAGGER):
		return WTAG_CRITICAL;

	case(WEAPON_QUARTERSTAFF):
		return WTAG_DOUBLE;

	case(WEAPON_RAPIER):
		return WTAG_LUNGE;

	case(WEAPON_SPEAR):
	case(WEAPON_WAR_PIKE):
		return WTAG_REACH;
	
	case(WEAPON_GREATSWORD):
	case(WEAPON_LONGSWORD):
		return WTAG_RIPOSTE;

	case(WEAPON_GREATMAUL):	
	case(WEAPON_MACE):
		return WTAG_STAGGER;
	
	case(WEAPON_WHIP):
		return WTAG_WHIP;

	default:
		return WTAG__NONE;
	}
}

int itemData::getDamageResist(const itid id)
{
	switch (id)
	{
	case(ARMOUR_CLOAK):			return 1;
	case(ARMOUR_ROBES):			return 2;
	case(ARMOUR_LEATHER):		return 4;
	case(ARMOUR_SCALE):			return 8;
	case(ARMOUR_CHAIN):			return 12;
	case(ARMOUR_PLATE):			return 16;
	default:
		return 0;
	}
}

int itemData::getEvasionAdjust(const itid id)
{
	switch (id)
	{
	case(ARMOUR_ROBES):			return 0;
	case(ARMOUR_LEATHER):		return -1;
	case(ARMOUR_SCALE):			return -2;
	case(ARMOUR_CHAIN):			return -4;
	case(ARMOUR_PLATE):			return -6;
	default:
		return 0;
	}
}

int itemData::getBlockValue(const itid id)
{
	switch (id)
	{
	case(SHIELD_BUCKLER):		return 6;
	case(SHIELD_KITE):			return 10;
	case(SHIELD_TOWER):			return 14;
	default:
		return 0;
	}
}

int itemData::getStatRequirement(const itid id)
{
	switch (id)
	{
		//	Constitution requirement to wear armour without penalty

	case(ARMOUR_ROBES):			return 1;
	case(ARMOUR_LEATHER):		return 6;
	case(ARMOUR_SCALE):			return 11;
	case(ARMOUR_CHAIN):			return 13;
	case(ARMOUR_PLATE):			return 16;


		//	Dexterity requirement to use shield without penalty

	case(SHIELD_BUCKLER):		return 10;
	case(SHIELD_KITE):			return 12;
	case(SHIELD_TOWER):			return 14;


		//	Strength requirement to wield weapon without penalty

	case(RANGED_COMPOSITE_BOW):	return 15;
	case(RANGED_LONGBOW):		return 12;
	case(RANGED_SHORTBOW):		return 8;

	case(WEAPON_DAGGER):		
	case(WEAPON_WHIP):
	case(WEAPON_RAPIER):
		return 4;

	case(WEAPON_SPEAR):
	case(WEAPON_QUARTERSTAFF):
		return 8;

	case(WEAPON_LONGSWORD):
	case(WEAPON_MACE):
		return 10;

	case(WEAPON_WAR_PIKE):
		return 11;

	case(WEAPON_WAR_AXE):
	case(WEAPON_GREATSWORD):
		return 13;

	case(WEAPON_GREATAXE):	
		return 15;

	case(WEAPON_GREATMAUL):
		return 16;

	}
	return 0;
}

int itemData::getGoldValue(const itid id)
{
	switch (id)
	{
	case(ARROWS_STANDARD):
		return 1;
	case(ARROWS_POISON):
	case(ARROWS_SERRATED):
	case(ARROWS_SILVERED):
		return 3;

	case(COATING_FLAME):
	case(COATING_ICE):
	case(COATING_LIGHTNING):
	case(COATING_POISON):
		return 10;

	case(POTION_HASTE):
	case(POTION_MIGHT):
	case(POTION_STONESKIN):
		return 25;

	case(SCROLL_PORTAL):
		return 50;

	case(URN_FIRE):
	case(URN_ICE):
	case(URN_LIGHTNING):
		return 8;

	case(URN_OIL):
	case(URN_WATER):
		return 4;
		
	default:
		return 1;
	}
}

string getWeaponTagName(const WeaponTag wtag)
{
	switch (wtag)
	{
	case(WTAG_CLEAVE):		return "cleave";
	case(WTAG_CRITICAL):	return "critical";
	case(WTAG_DOUBLE):		return "double";
	case(WTAG_LUNGE):		return "lunge";
	case(WTAG_REACH):		return "reach";
	case(WTAG_RIPOSTE):		return "riposte";
	case(WTAG_STAGGER):		return "stagger";
	case(WTAG_WHIP):		return "whip";

	case(WTAG_DROWNING):	return "drowning";
	default:
		return "weapon tag " + to_string(wtag);
	}
}
