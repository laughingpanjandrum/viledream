#pragma once

#include <memory>
#include "feats.h"
#include "itemData.h"


class item
{
public:

	item() {}
	item(itemData::itid id, int amt = 1);
	item(Spell sp);
	item(const item& other);
	~item() {}


	//	Flavour

	string getName() const;
	int getGlyph() const;
	colorType getColor() const;

	string getVerboseName() const;
	string getNameWithAmt() const;


	//	Equipment attributes

	void setQuality(const ItemQuality qual);

	int getStatRequirement() const { return itemData::getStatRequirement(_id); }
	Attribute getStatUsed() const;

	int getAccuracyAdjust() const { return _accAdjust; }
	int getAttackRange() const { return _attackRange; }
	intpair getDamageDice() const { return _damage; }
	int getDamageBonus() const { return _damageBonus; }

	WeaponTag getWeaponTag() const { return _weaponTag; }
	bool hasWeaponTag(const WeaponTag wtag) const { return _weaponTag == wtag; }

	int getEvadeAdjust() const { return _evAdjust; }
	int getDamageResist() const { return _damageResist; }
	int getBlockValue() const { return _blockValue; }


	//	Enchantments

	void setEnchantment(const Enchantment ench, const int bns);

	bool hasEnchantment() const { return _enchant != ENCH__NONE; }
	bool hasEnchantment(const Enchantment en) const { return _enchant == en; }
	int getEnchantBonus() const { return _enchantBonus; }
	Enchantment getEnchantment() const { return _enchant; }


	//	Spell powers

	bool grantsSpell() const { return _containsSpell != SPELL__NONE; }


	//	Stacking

	bool canStack() const { return _category == ITEM_AMMO || _category == ITEM_CONSUMABLE || _category == ITEM_GOLD || _category == ITEM_THROWN; }
	bool stacksWith(const item* with) const { return with->_id == _id; }


	//	public data

	Spell _containsSpell;

	int _amount;
	ItemCategory _category;
	ItemQuality _quality;
	itemData::itid _id;


protected:

	//	Equipment data

	int _evAdjust;
	int _accAdjust;
	intpair _damage;
	int _damageBonus;
	int _attackRange;

	int _damageResist;
	int _blockValue;

	WeaponTag _weaponTag;
	Enchantment _enchant;
	int _enchantBonus;

};
typedef shared_ptr<item> itemPtr;