#include "item.h"

item::item(itemData::itid id, int amt)
{
	_id = id;
	_category = itemData::getDefaultCategory(id);
	_amount = amt;
	_quality = ITEM_QUALITY_NORMAL;

	//	Default no enchantment
	_enchantBonus = 0;
	_enchant = ENCH__NONE;

	//	Equipment attributes
	_damage = itemData::getDamageDice(id);
	_damageBonus = 0;
	_damageResist = itemData::getDamageResist(id);
	_accAdjust = itemData::getAccuracyModifier(id);
	_evAdjust = itemData::getEvasionAdjust(id);
	_blockValue = itemData::getBlockValue(id);
	_attackRange = itemData::getAttackRange(id);
	_weaponTag = itemData::getWeaponTag(id);
}

item::item(Spell sp) : item(itemData::SPELLBOOK_NORMAL, 1)
{
	_containsSpell = sp;
	_category = ITEM_SPELLBOOK;
}

item::item(const item & other)
{
	_id = other._id;
	_category = other._category;
	_amount = other._amount;
	_quality = other._quality;

	_enchantBonus = other._enchantBonus;
	_enchant = other._enchant;

	_damage = other._damage;
	_damageBonus = other._damageBonus;
	_damageResist = other._damageResist;
	_accAdjust = other._accAdjust;
	_evAdjust = other._evAdjust;
	_blockValue = other._blockValue;
	_attackRange = other._attackRange;
	_weaponTag = other._weaponTag;
}

string item::getName() const
{
	if (_category == ITEM_SPELLBOOK)
		return "spellbook of " + getSpellName(_containsSpell);
	else
		return itemData::getName(_id);
}

int item::getGlyph() const
{
	return itemData::getGlyph(_id);
}

colorType item::getColor() const
{
	if (_category == ITEM_SPELLBOOK)
		return getSpellColor(_containsSpell);
	else
		return itemData::getColor(_id);
}

string item::getVerboseName() const
{
	//	base name of item
	string name = getName();

	//	quality tier
	if (_quality != ITEM_QUALITY_NORMAL)
		name = getItemQualityName(_quality) + ' ' + name;

	//	enchantment level
	if (hasEnchantment())
		name = plusminus(_enchantBonus) + ' ' + name;

	//	prefix
	if (canStack())
		name = "x" + to_string(_amount) + " " + name;
	else if (isVowel(name[0]))
		name = "an " + name;
	else
		name = "a " + name;

	//	enchantment type
	if (hasEnchantment())
		name += " of " + getEnchantmentName(_enchant);

	//	info about its basic stats
	switch (_category)
	{
	case(ITEM_ARMOUR):
		name += " (" + plusminus(_evAdjust) + "/" + to_string(_damageResist) + ")";
		break;

	case(ITEM_SHIELD):
		name += " (" + plusminus(_accAdjust) + "/" + to_string(_blockValue) + ")";
		break;

	case(ITEM_RANGED):
	case(ITEM_WEAPON):
		name += " (" + plusminus(_accAdjust) + " " + to_string(_damage.first) + "d" + to_string(_damage.second);
		if (_damageBonus != 0)
			name += plusminus(_damageBonus);
		name += ")";
		break;
	}

	return name;
}

string item::getNameWithAmt() const
{
	if (canStack())
		return to_string(_amount) + " " + getName();
	else
		return getName();
}


//	This modifies the item's attributes depending on its type.
void item::setQuality(const ItemQuality qual)
{
	_quality = qual;

	int bonus = 0;
	switch (qual)
	{
	case(ITEM_QUALITY_REFINED):		bonus = 1; break;
	case(ITEM_QUALITY_SUPERB):		bonus = 2; break;
	case(ITEM_QUALITY_MASTERWORK):	bonus = 3; break;
	}

	//	what we get depends on our type
	switch (_category)
	{
	case(ITEM_ARMOUR):
	case(ITEM_CLOAK):
		_damageResist += bonus; break;

	case(ITEM_SHIELD):
		_blockValue += bonus; break;

	case(ITEM_WEAPON):
		_accAdjust += bonus; 
		_damageBonus += bonus;
		break;
	}
}

Attribute item::getStatUsed() const
{
	switch (_category)
	{
	case(ITEM_ARMOUR):		return ATTR_CONSTITUTION;
	case(ITEM_SHIELD):		return ATTR_DEXTERITY;
	case(ITEM_RANGED):
	case(ITEM_WEAPON):		return ATTR_STRENGTH;
	default:
		return ATTR__MAX;
	}
}


//	Assign an enchantment type and bonus
void item::setEnchantment(const Enchantment ench, const int bns)
{
	_enchant = ench;
	_enchantBonus = bns;
}

int item::getGoldCost() const
{
	switch (_category)
	{
	case(ITEM_SPELLBOOK):
		return 100 * pow(2, getSpellCost(_containsSpell));

	default:
		return itemData::getGoldValue(_id) * _amount;
	}
}

