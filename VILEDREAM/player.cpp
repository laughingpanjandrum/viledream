#include "player.h"

player::player() : creature()
{
	//	Flavour
	_name = "Lost Prisoner";
	_glyph = '@';
	_color = COLOR_WHITE;

	//	Base attribute values
	for (unsigned i = 0; i < ATTR__MAX; i++)
		_attributes.push_back(10);
	
	//	Empty feat slots
	for (unsigned i = 0; i < PLAYER_MAX_FEAT_SLOTS; i++)
		_featSlots.push_back(FEAT__MAX);

	//	Starting state
	_baseHealthBonus = 24;
	_healingSurgesLeft = 1;
	_healingSurgeProgress = 0;
	_mpUsed = 0;

	//	Starting level
	_level = 1;
	_xp = 0;
}

int player::getDerivedAttributeValue(const Attribute attr) const
{
	int total = getBaseAttributeValue(attr);
	return total;
}

int player::getAttributeModifier(const Attribute attr) const
{
	auto val = getDerivedAttributeValue(attr);
	if (val < 10)
		return (val / 2) - 5;
	else
		return val - 10;
}

int player::getMaxHealth() const
{
	int total = _baseHealthBonus + 3 * getAttributeModifier(ATTR_CONSTITUTION);
	total = MAX(1, total);
	return total;
}

int player::getMaxMagicPoints() const
{
	int total = 1 + getAttributeModifier(ATTR_CHARISMA);
	total = MAX(0, total);
	return total;
}

int player::getBaseAccuracy() const
{
	int total = getAttributeModifier(ATTR_DEXTERITY);

	//	penalized due to missing stat requirements for equipment.
	if (_armour != nullptr)
		total -= getPenaltyFromStatRequirement(_armour);

	return total;
}

int player::getEvasion() const
{
	int total = 8 + getAttributeModifier(ATTR_AGILITY);

	//	bonus from item enchantments
	total += getTotalEnchantBonus(ENCH_EVASION);
	
	//	armour bonus
	if (_armour != nullptr)
		total += _armour->getEvadeAdjust();

	//	penalty for an ill-fitting shield
	if (_shield != nullptr)
		total -= getPenaltyFromStatRequirement(_shield);

	//	feats
	if (knowsFeat(FEAT_DANCERS_STRIDE) && _movedLastTurn)
		total += 2;

	//	ring bonus
	if (hasRingOfType(itemData::RING_DEFIANCE) && getHealthPercent() <= 30)
		total += 5;
	
	//	can't have sub-zero Evasion
	total = MAX(0, total);
	return total;
}

int player::getBaseDamageBonus() const
{
	int total = getAttributeModifier(ATTR_STRENGTH);
	return total;
}

int player::getMeleeAccuracy() const
{
	int total = creature::getMeleeAccuracy();
	
	//	weapon adjustments
	if (_weapon != nullptr)
	{
		total += _weapon->getAccuracyAdjust();
		total -= getPenaltyFromStatRequirement(_weapon);
	}
	if (_shield != nullptr)
	{
		int adj = _shield->getAccuracyAdjust();
		if (adj < 0 && knowsFeat(FEAT_SHIELD_PROFICIENCY))
			adj = MIN(0, adj + 1);
		total += adj;
	}
	
	//	ring bonus
	if (hasRingOfType(itemData::RING_MIGHTY_BLOWS))
		total -= 2;
	
	return total;
}

intpair player::getMeleeDamageDice() const
{
	if (_weapon != nullptr)
	{
		auto dice = _weapon->getDamageDice();
		if (knowsFeat(FEAT_VITAL_STRIKE))
			dice.first *= 2;
		return dice;
	}
	else
		return intpair(1, 2);
}


//	Added to damage from all melee attacks
int player::getMeleeDamageBonus() const
{
	int total = creature::getMeleeDamageBonus();

	//	weapon bonus, on top of its dice
	if (_weapon != nullptr)
		total += _weapon->getDamageBonus();

	//	feat bns
	if (knowsFeat(FEAT_DANCE_OF_DEATH) && _movedLastTurn)
		total += 3;
	
	//	ring bonus
	if (hasRingOfType(itemData::RING_MIGHTY_BLOWS))
		total += 4;
	if (hasRingOfType(itemData::RING_DAMNATION) && getHealthPercent() <= 30)
		total += 7;

	return total;
}

//	Chance out of 100 per hit to score a critical hit
int player::getMeleeCritChance() const
{
	return 5;
}


//	Damage multiplier on a critical hit
int player::getMeleeCritMultiplier() const
{
	int mult = 2;
	if (_weapon != nullptr && _weapon->hasWeaponTag(WTAG_CRITICAL))
		mult += 3;
	return mult;
}

int player::getRangedAccuracy() const
{
	int total = getBaseAccuracy();

	//	weapon modifiers
	if (_ranged != nullptr)
	{
		total += _ranged->getAccuracyAdjust();
		total -= getPenaltyFromStatRequirement(_ranged);
	}
	
	return total;
}

intpair player::getRangedDamageDice() const
{
	if (_ranged != nullptr)
		return _ranged->getDamageDice();
	return intpair(0, 0);
}

int player::getRangedDamageBonus() const
{
	int total = 0;
	
	if (_ranged != nullptr)
	{
		if (_ranged->_id == itemData::RANGED_COMPOSITE_BOW)
			total += getBaseDamageBonus();
		total += _ranged->getDamageBonus();
	}
	
	return total;
}

int player::getAttackRange() const
{
	if (_ranged != nullptr)
		return itemData::getAttackRange(_ranged->_id);
	return 0;
}

int player::getAttackActionDelay() const
{
	int del = creature::getAttackActionDelay();
	if (knowsFeat(FEAT_VITAL_STRIKE))
		del = adjustActionDelay(del, -1);
	return del;
}

int player::getDamageResist() const
{
	int total = 0;

	if (_armour != nullptr)
		total += _armour->getDamageResist();
	if (_cloak != nullptr)
		total += _cloak->getDamageResist();

	if (hasBuff(BUFF_STONESKIN))
		total += 10;

	return total;
}

int player::getBlockValue() const
{
	int total = 0;
	total += getTotalEnchantBonus(ENCH_DEFLECTION);
	
	if (_shield != nullptr)
	{
		total += _shield->getBlockValue();
		if (knowsFeat(FEAT_SHIELD_PROFICIENCY)) total += 2;
		total -= getPenaltyFromStatRequirement(_shield) * 2;
	}
	
	//	we can't have less than zero Block
	total = MAX(0, total);
	return total;
}

int player::getResistance(const ResistanceType res) const
{
	int total = creature::getResistance(res);
	total += 10 + getAttributeModifier(ATTR_CONSTITUTION);

	//	for specific resistances
	switch (res)
	{
	case(RESIST_AGONY):
	case(RESIST_BLEED):
	case(RESIST_FEAR):
	case(RESIST_PETRIFY):
	case(RESIST_POISON):
	case(RESIST_STAGGER):
		total += 5 * getTotalEnchantBonus(ENCH_RESISTANCE);
	}
	return total;
}

bool player::canRiposte() const
{
	return _weapon != nullptr && _weapon->hasWeaponTag(WTAG_RIPOSTE);
}

bool player::hasMeleeWeaponTag(const WeaponTag wtag) const
{
	if (_weapon != nullptr)
		return _weapon->hasWeaponTag(wtag);
	return false;
}

intpair player::getMeleeSpecialDamage(const DamageType dt) const
{
	//	from coatings
	switch (dt)
	{
	case(DTYPE_COLD):
		if (hasBuff(BUFF_COATING_FROST))
			return intpair(1, 6);
		break;

	case(DTYPE_ELECTRIC):
		if (hasBuff(BUFF_COATING_LIGHTNING))
			return intpair(1, 6);
		break;

	case(DTYPE_FIRE):
		if (hasBuff(BUFF_COATING_FLAME))
			return intpair(1, 6);
		break;
	}

	return intpair(0, 0);
}

int player::getMeleeAfflictionStacks(const Affliction af) const
{
	int total = 0;
	switch (af)
	{
	case(AFFL_POISON):
		if (hasBuff(BUFF_COATING_POISON))
			total += 5;
	}
	return total;
}

itemData::itid player::getArrowTypeUsed() const
{
	if (_ammo != nullptr)
		return _ammo->_id;
	return itemData::ITEM__ID__NONE;
}


const vector<ReprisalAttack> player::getReprisalAttacks() const
{
	vector<ReprisalAttack> all;

	//	cloak of blades
	int blades = getTotalEnchantBonus(ENCH_BLADES);
	if (blades > 0)
		all.push_back(ReprisalAttack(intpair(1, blades * 2)));

	//	ring of chill
	if (hasRingOfType(itemData::RING_CHILL_AURA))
		all.push_back(ReprisalAttack(intpair(1, 4), DTYPE_COLD));

	return all;
}


//	Returns True if we meet the stat requirement for this item.
bool player::meetsStatRequirement(itemPtr it) const
{
	auto attr = it->getStatUsed();
	if (attr != ATTR__MAX)
		return getDerivedAttributeValue(attr) >= it->getStatRequirement();
	else
		return true;
}


//	Penalty to the item's usefulness due to not meeting its stat requirement.
//	Zero if we meet the requirement or it has none; otherwise, a positive number, to be deducted from (something).
int player::getPenaltyFromStatRequirement(itemPtr it) const
{
	auto attr = it->getStatUsed();
	if (attr != ATTR__MAX)
	{
		auto val = it->getStatRequirement() - getDerivedAttributeValue(attr);
		return MAX(0, val);
	}
	else
		return 0;
}


//	Returns a list of items we have equipped. Elements are 'nullptr' if no item is equipped in that slot.
//	Ammunition is NOT included in this list, because it's more like a consumable than an equipment type.
const vector<itemPtr> player::getAllEquippedItems() const
{
	return { _weapon, _ranged, _shield, _armour, _cloak, _leftRing, _rightRing };
}


//	Returns a list of which category each item in the list given by 'getAllEquippedItems' corresponds to.
const vector<ItemCategory> player::getEquippedItemCategories() const
{
	return { ITEM_WEAPON, ITEM_RANGED, ITEM_SHIELD, ITEM_ARMOUR, ITEM_CLOAK, ITEM_RING, ITEM_RING };
}


//	Returns our primary equipped item of the given type, if we have one.
itemPtr player::getItemInSlot(ItemCategory cat) const
{
	switch (cat)
	{
	case(ITEM_AMMO):		return _ammo;
	case(ITEM_ARMOUR):		return _armour;
	case(ITEM_CLOAK):		return _cloak;
	case(ITEM_RANGED):		return _ranged;
	case(ITEM_RING):		return _leftRing;
	case(ITEM_SHIELD):		return _shield;
	case(ITEM_WEAPON):		return _weapon;
	default:
		return nullptr;
	}
}


//	Returns our ALTERNATE item of the given category, if we have one.
//	This is either our alt weapon or our right-hand ring.
itemPtr player::getAltItem(ItemCategory cat) const
{
	switch (cat)
	{
	case(ITEM_RING):		return _rightRing;
	case(ITEM_WEAPON):		return _altWeapon;
	default:
		return nullptr;
	}
}


//	Puts the item in the appropriate inventory slot, replacing whatever is there, if anything
void player::equipItem(itemPtr it)
{
	switch (it->_category)
	{
	case(ITEM_AMMO):		_ammo = it; break;
	case(ITEM_ARMOUR):		_armour = it; break;
	case(ITEM_CLOAK):		_cloak = it; break;
	case(ITEM_RANGED):		_ranged = it; break;
	case(ITEM_RING):		_leftRing = it; break;
	case(ITEM_SHIELD):		_shield = it; break;
	case(ITEM_WEAPON):		_weapon = it; break;
	}
}

void player::unequipItem(itemPtr it)
{
	if		(_ammo == it)		_ammo = nullptr;
	else if (_armour == it)		_armour = nullptr;
	else if (_cloak == it)		_cloak = nullptr;
	else if (_ranged == it)		_ranged = nullptr;
	else if (_shield == it)		_shield = nullptr;
	else if (_weapon == it)		_weapon = nullptr;
	else if (_leftRing == it)	_leftRing = nullptr;
	else if (_rightRing == it)	_rightRing = nullptr;
}


//	Returns true if either of our rings are of the given type
//	Rings of the same type don't stack, so we only need to know if we have one!
bool player::hasRingOfType(itemData::itid id) const
{
	return (_leftRing != nullptr && _leftRing->_id == id) || (_rightRing != nullptr && _rightRing->_id == id);
}


//	Total enchantment bonus from all of our equipment
int player::getTotalEnchantBonus(const Enchantment en) const
{
	int total = 0;
	for (auto it : { _armour, _cloak, _shield, _weapon, _ranged })
	{
		if (it != nullptr && it->hasEnchantment(en))
			total += it->getEnchantBonus();
	}
	return total;
}


int player::getMaxHealingSurges() const
{
	int total = 3;
	if (hasRingOfType(itemData::RING_LIFE))
		total++;
	return total;
}


int player::getHealingSurgeProgressRate() const
{
	int total = 15;
	if (hasRingOfType(itemData::RING_BLOOD_HUNGER))
		total += 10;
	return total;
}

void player::gainHealingSurgeProgress()
{
	//	we can only gain surges if we aren't at the cap
	if (_healingSurgesLeft < getMaxHealingSurges())
	{
		int amt = getHealingSurgeProgressRate();
		_healingSurgeProgress += amt + randint(-amt / 2, amt / 2);
		if (_healingSurgeProgress >= 100)
		{
			addHealingSurge();
			_healingSurgeProgress -= 100;
		}
	}

	//	if we have Arcane Thirst, this also generates some MP
	if (hasRingOfType(itemData::RING_ARCANE_THIRST))
		tickMpRegen(randint(2, 5));
}


//	Proportion of maximum health healed by a surge.
float player::getAmountHealedBySurge() const
{
	if (hasRingOfType(itemData::RING_HEALER))
		return 0.4f;
	else
		return 0.3f;
}

void player::addHealingSurge(const int amt)
{
	if (_healingSurgesLeft < getMaxHealingSurges())
		_healingSurgesLeft++;
}


//	Expend a healing surge (if we have one) and gain health
void player::useHealingSurge()
{
	if (_healingSurgesLeft > 0)
	{
		_healingSurgesLeft--;
		healDamage((float)getMaxHealth() * getAmountHealedBySurge());
	}
}

//	Returns true if we have any knowledge of the given spell
bool player::knowsSpell(const Spell sp) const
{
	return getSpellCastsLeft(sp) > 0;
}

//	Return remaining castings of this spell, if any
int player::getSpellCastsLeft(const Spell sp) const
{
	auto f = find(_spellsKnown.begin(), _spellsKnown.end(), sp);
	if (f != _spellsKnown.end())
		return _spellCastsLeft[f - _spellsKnown.begin()];
	return 0;
}

//	Add additional castings of a given spell
void player::addSpellKnowledge(const Spell sp, const int casts)
{
	//	if we already know the spell, increase remaining castings
	auto f = find(_spellsKnown.begin(), _spellsKnown.end(), sp);
	if (f != _spellsKnown.end())
		_spellCastsLeft[f - _spellsKnown.begin()] += casts;

	//	this is a new spell to us
	else
	{
		_spellsKnown.push_back(sp);
		_spellCastsLeft.push_back(casts);
	}
}

//	Lose 1 casting of the spell.
void player::spendSpellKnowledge(const Spell sp)
{
	auto f = find(_spellsKnown.begin(), _spellsKnown.end(), sp);
	if (f != _spellsKnown.end())
		_spellCastsLeft[f - _spellsKnown.begin()] -= 1;
}


//	If we are lacking magic points, slowly regenerated them.
//	'portion' is what % of a charge is restored. If set to zero, uses our inherent MP regen.
void player::tickMpRegen(int portion)
{
	if (_mpUsed > 0)
	{
		//	how much regen
		if (portion == 0)
			portion = getMPRegen();
		_mpRegenProgress += portion;

		//	if we pass the limit, get a charge
		if (_mpRegenProgress >= 100)
		{
			_mpRegenProgress -= 100;
			_mpUsed--;
		}
	}
}


//	Maximum number of feats we can have equipped
int player::getMaxFeatsKnown() const
{
	return _level / 4;
}


//	Returns True if we can learn a feat for the given slot.
bool player::isFeatSlotAvailable(const int idx) const
{
	return idx >= 0 && idx < _featSlots.size() && idx < getMaxFeatsKnown() && _featSlots[idx] == FEAT__MAX;
}


//	Returns True if we have learned the given feat.
bool player::knowsFeat(const Feat ft) const
{
	return find(_featSlots.begin(), _featSlots.end(), ft) != _featSlots.end();
}


//	Places feat in the first empty slot/
//	If there are no empty slots, does nothing.
void player::learnFeat(const Feat ft)
{
	for (unsigned i = 0; i < _featSlots.size(); i++)
	{
		if (_featSlots[i] == FEAT__MAX)
		{
			_featSlots[i] = ft;
			return;
		}
	}
}
