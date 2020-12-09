#pragma once

#include "gamedata.h"
#include "messages.h"
#include "monster.h"
#include "progression.h"
#include "visibility.h"


//	Surfaces

bool testForSurfacePair(const surface s1, const surface s2, const surface t1, const surface t2);
surface getSurfaceCollision(const surface base, const surface sf);
surface getSurfaceAndTileCollision(const maptile tl, const surface sf);
void tryAddSurface(gamedataPtr gdata, surface sf, intpair pt);

void applySurfaceToRegion(gamedataPtr gdata, surface sf, const intpair ctr, const int sz);

bool creatureSubjectToDrowning(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);


//	Special effects

void queueMonsterSpawn(gamedataPtr gdata, const string id, const intpair pt, const int with_damage = 0);

void splitOnTakeDamage(gamedataPtr gdata, monsterPtr mon);

void doReprisalAttacks(gamedataPtr gdata, creaturePtr repriser, creaturePtr target);



//	Death and damage

void getMonsterKillRewards(gamedataPtr gdata, monsterPtr mon);
void doMonsterDeathEffects(gamedataPtr gdata, monsterPtr mon);

void killCreature(gamedataPtr gdata, creaturePtr target);
void creatureTakeDamage(gamedataPtr gdata, creaturePtr target, int amt);

void inflictAgonyDamage(gamedataPtr gdata, creaturePtr target);
void inflictDrowning(gamedataPtr gdata, creaturePtr target);
void inflictEnergyDamage(gamedataPtr gdata, creaturePtr target, const DamageType dt, int amt);
void tryAddAffliction(gamedataPtr gdata, creaturePtr target, Affliction af, int stacks);


//	Triggering attacks

bool rollToEvade(gamedataPtr gdata, int acc, creaturePtr target);
bool rollToBlock(gamedataPtr gdata, int acc, creaturePtr target);
int applyDamageResist(gamedataPtr gdata, creaturePtr target, int dam);

void applyMeleeHitEffects(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);
void applyRangedHitEffects(gamedataPtr gdata, creaturePtr attacker, creaturePtr target);
void triggerWeaponAttack(gamedataPtr gdata, creaturePtr attacker, creaturePtr target, bool is_melee = true, bool is_riposte = false, int accAdjust = 0, int dmgAdjust = 0);

void checkForLungeAttack(gamedataPtr gdata, creaturePtr mover, intpair vec);
void triggerCleaveAttack(gamedataPtr gdata, creaturePtr attacker);
void triggerReachAttack(gamedataPtr gdata, creaturePtr attacker, intpair vec);

void triggerTouchAttack(gamedataPtr gdata, creaturePtr attacker, creaturePtr target, const Affliction af, const int stacks);


//	Special weapons

void applyArrowEffect(gamedataPtr gdata, creaturePtr target, const itemData::itid id);

void applyThrownWeaponEffect(gamedataPtr gdata, itemData::itid id, intpair pt);
void hitPointWithThrownWeapon(gamedataPtr gdata, itemPtr thrown, intpair pt);

void triggerThrownAttack(gamedataPtr gdata, creaturePtr thrower, itemPtr thrown, intpair at);


//	Player actions

void playerUseHealingSurge(gamedataPtr gdata);

void playerTriggerRangedAttack(gamedataPtr gdata, const intpair at);
void playerTriggerMeleeAttack(gamedataPtr gdata, creaturePtr target);

void playerAimRanged(gamedataPtr gdata);
void playerAimThrown(gamedataPtr gdata, const int idx);