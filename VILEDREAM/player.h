#pragma once

#include "creature.h"
#include "item.h"


#define PLAYER_MAX_FEAT_SLOTS 10


class player : public creature
{
public:

	player();
	~player() {}

	virtual bool isPlayer() const { return true; }


	//	Base attributes

	int getBaseAttributeValue(const Attribute attr) const { return _attributes[attr]; }
	int getDerivedAttributeValue(const Attribute attr) const;

	int getAttributeModifier(const Attribute attr) const;


	//	Improvements

	void increaseBaseHealth(int amt) { _baseHealthBonus += amt; }
	void increaseAttribute(const Attribute attr, const int amt = 1) { _attributes[attr] += amt; }


	//	Health and magic

	virtual int getMaxHealth() const;
	virtual int getMaxMagicPoints() const;


	//	Attacks

	virtual int getBaseAccuracy() const;
	virtual int getEvasion() const;
	virtual int getBaseDamageBonus() const;

	virtual int getMeleeAccuracy() const;
	virtual intpair getMeleeDamageDice() const;
	virtual int getMeleeDamageBonus() const;
	virtual int getMeleeCritChance() const;
	virtual int getMeleeCritMultiplier() const;

	virtual int getRangedAccuracy() const;
	virtual intpair getRangedDamageDice() const;
	virtual int getRangedDamageBonus() const;
	virtual int getAttackRange() const;

	virtual int getAttackActionDelay() const;


	//	Defences

	virtual int getDamageResist() const;
	virtual int getBlockValue() const;
	virtual int getResistance(const ResistanceType res) const;


	//	Special moves

	virtual bool canRiposte() const;
	virtual bool hasMeleeWeaponTag(const WeaponTag wtag) const;
	virtual intpair getMeleeSpecialDamage(const DamageType dt) const;
	virtual int getMeleeAfflictionStacks(const Affliction af) const;

	virtual itemData::itid getArrowTypeUsed() const;

	virtual const vector<ReprisalAttack> getReprisalAttacks() const;


	//	Equipment

	itemPtr _ammo;
	itemPtr _armour;
	itemPtr _cloak;
	itemPtr _ranged;
	itemPtr _shield;
	itemPtr _weapon;

	itemPtr _altWeapon;
	itemPtr _leftRing, _rightRing;

	bool meetsStatRequirement(itemPtr it) const;
	int getPenaltyFromStatRequirement(itemPtr it) const;

	const vector<itemPtr> getAllEquippedItems() const;
	const vector<ItemCategory> getEquippedItemCategories() const;

	itemPtr getItemInSlot(ItemCategory cat) const;
	itemPtr getAltItem(ItemCategory cat) const;
	void equipItem(itemPtr it);
	void unequipItem(itemPtr it);

	bool hasRingOfType(itemData::itid id) const;
	int getTotalEnchantBonus(const Enchantment en) const;


	//	Healing surges

	int getMaxHealingSurges() const;
	int getHealingSurgesLeft() const { return _healingSurgesLeft; }
	bool hasHealingSurge() const { return _healingSurgesLeft > 0; }
	bool atMaxHealingSurges() const { return _healingSurgesLeft >= getMaxHealingSurges(); }

	int getHealingSurgeProgressRate() const;
	void gainHealingSurgeProgress();
	int getHealingSurgeProgress() const { return _healingSurgeProgress; }

	float getAmountHealedBySurge() const;
	void addHealingSurge(const int amt = 1);
	void useHealingSurge();


	//	Magic

	bool knowsSpell(const Spell sp) const;
	int getSpellCastsLeft(const Spell sp) const;

	void addSpellKnowledge(const Spell sp, const int casts);
	void spendSpellKnowledge(const Spell sp);

	int getMagicPointsLeft() const { return getMaxMagicPoints() - _mpUsed; }
	bool hasMagicPointsLeft(int amt) const { return getMagicPointsLeft() >= amt; }
	void loseMagicPoints(int amt) { _mpUsed += amt; }

	int getMPRegen() const { return 10; }
	void tickMpRegen(int portion = 0);


	vector<Spell> _spellsKnown;
	vector<int> _spellCastsLeft;


	//	Feats

	vector<Feat> _featSlots;

	int getMaxFeatsKnown() const;
	bool isFeatSlotAvailable(const int idx) const;

	virtual bool knowsFeat(const Feat ft) const;
	void learnFeat(const Feat ft);



	//	Public data

	int _xp;
	int _level;

	int _mpUsed;


protected:

	vector<int> _attributes;
	int _baseHealthBonus;

	int _healingSurgeProgress;		//	progress out of 100 to our next surge
	int _healingSurgesLeft;			//	no. of surges available

	int _mpRegenProgress;			//	progress out of 100 to gain our next point of MP

};
typedef shared_ptr<player> playerPtr;