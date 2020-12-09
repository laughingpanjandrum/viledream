#pragma once

#include <string>
#include <vector>

#include "enums.h"
#include "color_constants.h"

using namespace std;



//---------------------------------------
//	Feats

enum Feat {
	FEAT_AXE_PROFICIENCY,
	FEAT_BLADE_PROFICIENCY,
	FEAT_DANCERS_STRIDE,
	FEAT_DANCE_OF_DEATH,
	FEAT_DUALSTRIKE,
	FEAT_MACE_PROFICIENCY,
	FEAT_PUSHING_SHOT,
	FEAT_SHIELD_PROFICIENCY,
	FEAT_SPEAR_PROFICIENCY,
	FEAT_VITAL_STRIKE,
	FEAT__MAX,
};

const vector<Feat> ALL_FEATS = {
	FEAT_AXE_PROFICIENCY, FEAT_BLADE_PROFICIENCY, FEAT_MACE_PROFICIENCY, FEAT_SHIELD_PROFICIENCY, FEAT_SPEAR_PROFICIENCY,
	FEAT_DANCERS_STRIDE, FEAT_DANCE_OF_DEATH, FEAT_DUALSTRIKE, FEAT_PUSHING_SHOT, FEAT_VITAL_STRIKE,
};

string getFeatName(const Feat ft);
string getFeatDescription(const Feat ft);
bool isFeatMutation(const Feat ft);



//---------------------------------------
//	Spells


enum Spell {
	SPELL_ARCANE_EMANATION,
	SPELL_BLOOD_SPEW,
	SPELL_CALL_LIGHTNING,
	SPELL_CRYSTAL_SPEAR,
	SPELL_ELECTRIFY,
	SPELL_EXCRUCIATION,
	SPELL_FIREBALL,
	SPELL_FRIGID_BLAST,
	SPELL_ICE_SPIKE,
	SPELL_LIGHTNING_SPEAR,
	SPELL_SLAY_LIVING,
	SPELL_THROW_FIRE,
	SPELL__NONE,
};


string getSpellName(const Spell sp);
string getSpellDescription(const Spell sp);
colorType getSpellColor(const Spell sp);
int getSpellCost(const Spell sp);

bool isTargetedSpell(const Spell sp);
int getSpellRange(const Spell sp);