#pragma once

#include "creature.h"


class monster : public creature
{
public:

	monster();
	monster(const vector<string> tokens);
	~monster() {}


	//	Derived attributes

	virtual int getMaxHealth() const { return _maxHealth; }
	virtual int getMaxMagicPoints() const { return 0; }

	virtual int getBaseAccuracy() const { return 0; }
	virtual int getEvasion() const { return _evasion; }
	virtual int getBaseDamageBonus() const { return 0; }

	virtual int getMeleeAccuracy() const;
	virtual intpair getMeleeDamageDice() const { return _dmgMelee; }
	virtual int getMeleeDamageBonus() const { return _meleeDmgBonus; }

	virtual int getRangedAccuracy() const;
	virtual intpair getRangedDamageDice() const { return _dmgRanged; }
	virtual int getRangedDamageBonus() const { return 0; }

	virtual int getDamageResist() const { return _damageResist; }
	virtual int getBlockValue() const { return _blockValue; }

	virtual int getResistance(const ResistanceType res) const;


	//	Special properties / attacls

	virtual bool hasMeleeWeaponTag(const WeaponTag wtag) const;
	virtual intpair getMeleeSpecialDamage(const DamageType dt) const;
	virtual int getMeleeAfflictionStacks(const Affliction af) const;
	
	virtual bool grapplesOnHit() const { return hasFlag("grapples"); }
	virtual bool splitOnTakeDamage() const { return hasFlag("split_on_take_damage"); }
	
	virtual bool immuneToDrowning() const;
	virtual bool isFlying() const { return hasFlag("flying"); }
	virtual bool isUndead() const { return hasFlag("undead"); }

	virtual string spawnsOnDeath() const { return _spawnsOnDeath; }
	virtual string spawns() const { return _spawns; }


	//	Behaviours

	virtual int getMoveActionDelay() const;


	bool hasTarget() const { return _target != nullptr; }
	bool hasFlag(const string flag) const { return find(_flags.begin(), _flags.end(), flag) != _flags.end(); }


	//	Public data

	intpair _targetPt;
	creaturePtr _target;

	string _id;
	int _dangerLevel;


private:

	intpair dieFromString(const string val);
	void setAttribute(const string tag, const string val);
	void setMeleeSpecialDamage(const DamageType dt, const intpair die);

	//	attributes (these are defined, unlike player stats which are usually derived)
	int _maxHealth;
	int _accMelee, _accRanged;
	int _meleeDmgBonus;
	intpair _dmgMelee, _dmgRanged;
	int _evasion, _blockValue, _damageResist;
	vector<string> _flags;

	//	secondary properties
	intpair _meleeSpecialDamage;
	DamageType _meleeSpecialDamageType;

	//	special abilities
	string _spawnsOnDeath;
	string _spawns;
};
typedef shared_ptr<monster> monsterPtr;