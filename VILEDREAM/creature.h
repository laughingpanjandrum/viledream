#pragma once

#include <memory>
#include "enums.h"
#include "item.h"


#define ACTION_DELAY_FAST	1
#define ACTION_DELAY_NORMAL	2
#define ACTION_DELAY_SLOW	4


const int adjustActionDelay(const int base_delay, const int adj);


struct ReprisalAttack
{
	ReprisalAttack() {}
	ReprisalAttack(intpair dam, DamageType dt = DTYPE_PHYSICAL) : _damage(dam), _dtype(dt) {}
	intpair _damage;
	DamageType _dtype;
};


class creature
{
public:

	creature();
	~creature() {}


	virtual bool isPlayer() const { return false; }
	virtual bool isHostile() const { return false; }


	//	Flavour getters

	virtual string getName() const { return _name; }
	virtual int getGlyph() const { return _glyph; }
	virtual colorType getColor() const { return _color; }


	//	State

	int getHealthLeft() const { return getMaxHealth() - _damageTaken; }
	int getHealthPercent() const;


	//	Damage and death

	void takeDamage(int amt) { _damageTaken += amt; }
	bool isDead() const { return _damageTaken >= getMaxHealth(); }

	void healDamage(int amt);


	//	Afflictions

	bool hasAffliction(const Affliction af) const { return _afflictions[af] > 0; }
	int getAfflictionStacks(const Affliction af) const { return _afflictions[af]; }

	void addAfflictionStacks(const Affliction af, int stacks = 1);

	void tickAfflictions();


	//	Buffs

	bool hasBuff(const BuffType bf) const { return _buffs[bf] > 0; }
	int getBuffDuration(const BuffType bf) const { return _buffs[bf]; }

	void stackBuff(const BuffType bf, int duration, int upTo = -1);
	void addBuff(const BuffType bf, int duration);
	void tickBuffs();


	//	Conditions

	bool hasCondition(const Condition cond) const { return _conditions[cond] > 0; }
	int getConditionDuration(const Condition cond) const { return _conditions[cond]; }

	void addCondition(const Condition cond, int duration);
	void tickConditions();



	//	Derived attributes

	virtual int getMaxHealth() const = 0;
	virtual int getMaxMagicPoints() const = 0;

	virtual int getBaseAccuracy() const = 0;
	virtual int getEvasion() const = 0;
	virtual int getBaseDamageBonus() const = 0;

	virtual int getMeleeAccuracy() const;
	virtual intpair getMeleeDamageDice() const = 0;
	virtual int getMeleeDamageBonus() const;
	virtual int getMeleeCritChance() const { return 0; }
	virtual int getMeleeCritMultiplier() const { return 1; }

	virtual int getRangedAccuracy() const = 0;
	virtual intpair getRangedDamageDice() const = 0;
	virtual int getRangedDamageBonus() const = 0;
	virtual int getAttackRange() const { return 1; }

	virtual int getDamageResist() const = 0;
	virtual int getBlockValue() const = 0;


	//	Special moves

	virtual int getMeleeAttackCount() const;

	virtual bool canRiposte() const { return false; }
	virtual bool hasMeleeWeaponTag(const WeaponTag wtag) const { return false; }

	virtual intpair getMeleeSpecialDamage(const DamageType dt) const;
	virtual int getMeleeAfflictionStacks(const Affliction af) const { return 0; }

	virtual itemData::itid getArrowTypeUsed() const { return itemData::ITEM__ID__NONE; }

	virtual const vector<ReprisalAttack> getReprisalAttacks() const { return {}; }

	virtual bool knowsFeat(const Feat ft) const { return false; }


	//	Resistances

	virtual int getResistance(const ResistanceType res) const;

	virtual bool immuneToDrowning() const { return false; }
	virtual bool isUndead() const { return false; }


	//	Special statuses/abilities

	virtual bool grapplesOnHit() const { return false; }
	virtual bool splitOnTakeDamage() const { return false; }
	
	virtual bool isFlying() const { return false; }
	virtual bool canMove() const { return !isGrappled(); }


	//	Grappling

	bool isGrappled() const { return _grappler != nullptr; }
	void setGrappler(shared_ptr<creature> who) { _grappler = who; }
	void verifyGrappler();


	//	Action delay

	virtual int getMoveActionDelay() const;
	virtual int getAttackActionDelay() const;

	bool isActionDelayClear() const { return _actionDelay < 1; }
	void setActionDelay() { setActionDelayTo(ACTION_DELAY_NORMAL); }
	void setActionDelayTo(const int val);

	void tickActionDelay();


	//	public data

	intpair _pos;
	int _damageTaken;
	int _actionDelay;

	bool _movedLastTurn;


protected:

	string _name;
	int _glyph;
	colorType _color;

	vector<int> _afflictions;
	vector<int> _buffs;
	vector<int> _conditions;
	vector<int> _resistances;

	shared_ptr<creature> _grappler;

};
typedef shared_ptr<creature> creaturePtr;