#include "feats.h"

string getFeatName(const Feat ft)
{
	switch (ft)
	{
	case(FEAT_AXE_PROFICIENCY):			return "Axe Proficiency";
	case(FEAT_BLADE_PROFICIENCY):		return "Blade Proficiency";
	case(FEAT_DANCERS_STRIDE):			return "Dancer's Stride";
	case(FEAT_DANCE_OF_DEATH):			return "Dance of Death";
	case(FEAT_DUALSTRIKE):				return "Dualstrike";
	case(FEAT_MACE_PROFICIENCY):		return "Mace Proficiency";
	case(FEAT_PUSHING_SHOT):			return "Pushing Shot";
	case(FEAT_SHIELD_PROFICIENCY):		return "Shield Proficiency";
	case(FEAT_SPEAR_PROFICIENCY):		return "Spear Proficiency";
	case(FEAT_VITAL_STRIKE):			return "Vital Strike";
	default:
		return "feat " + to_string(ft);
	}
}

string getFeatDescription(const Feat ft)
{
	switch (ft)
	{
	case(FEAT_AXE_PROFICIENCY):
		return "Cleaving attacks inflict +1 damage for every enemy adjacent to you.";
	case(FEAT_BLADE_PROFICIENCY):
		return "Ripostes made with swords have +2 accuracy and inflict +2 damage.";
	case(FEAT_DANCERS_STRIDE):
		return "You get +2 EV if you moved on your last turn.";
	case(FEAT_DANCE_OF_DEATH):
		return "You inflict +3 melee damage if you moved on your last turn.";
	case(FEAT_DUALSTRIKE):
		return "You get an extra melee attack, but have -4 melee accuracy.";
	case(FEAT_MACE_PROFICIENCY):
		return "Hits with maces inflict twice as much Stagger.";
	case(FEAT_PUSHING_SHOT):
		return "Ranged attacks have a 30% chance to stagger the target.";
	case(FEAT_SHIELD_PROFICIENCY):
		return "Reduces the accuracy penalty conferred by worn shields, and increases block chance.";
	case(FEAT_SPEAR_PROFICIENCY):
		return "Reach attacks have +2 accuracy.";
	case(FEAT_VITAL_STRIKE):
		return "Your melee attacks double their damage dice, but are slow.";
	default:
		return "feat " + to_string(ft);
	}
}

bool isFeatMutation(const Feat ft)
{
	return false;
}

string getSpellName(const Spell sp)
{
	switch (sp)
	{
	case(SPELL_ARCANE_EMANATION):		return "Arcane Emanation";
	case(SPELL_BLOOD_SPEW):				return "Blood Spew";
	case(SPELL_CALL_LIGHTNING):			return "Call Lightning";
	case(SPELL_CRYSTAL_SPEAR):			return "Crystal Spear";
	case(SPELL_ELECTRIFY):				return "Electrify";
	case(SPELL_EXCRUCIATION):			return "Excruciation";
	case(SPELL_FIREBALL):				return "Fireball";
	case(SPELL_FRIGID_BLAST):			return "Frigid Blast";
	case(SPELL_ICE_SPIKE):				return "Ice Spike";
	case(SPELL_LIGHTNING_SPEAR):		return "Lightning Spear";
	case(SPELL_SLAY_LIVING):			return "Slay Living";
	case(SPELL_THROW_FIRE):				return "Throw Fire";
	default:
		return "spell " + to_string(sp);
	}
}

string getSpellDescription(const Spell sp)
{
	switch (sp)
	{
	case(SPELL_ARCANE_EMANATION):
		return "Fires an unerring beam of arcane energy that strikes a random target, inflicting 2d4+2 ethereal damage.";
	case(SPELL_BLOOD_SPEW):
		return "Spray of blood inflicts 8d6 damage in a line, but you take 2d6 damage upon casting.";
	case(SPELL_CALL_LIGHTNING):
		return "Lightning strikes a random enemy for 1d12 electric damage.";
	case(SPELL_CRYSTAL_SPEAR):
		return "Fire a penetrating crystalline spear that inflicts 2d12 ethereal damage in a line.";
	case(SPELL_ELECTRIFY):
		return "All surfaces near you are electrified. Stand still!";
	case(SPELL_EXCRUCIATION):
		return "All visible enemies suffer Agony.";
	case(SPELL_FIREBALL):
		return "You release an explosion of fire, inflicting 3d6 fire damage to everything near you.";
	case(SPELL_FRIGID_BLAST):
		return "Fire a short-ranged blast of ice that inflicts 3d6 cold damage and knockback.";
	case(SPELL_ICE_SPIKE):
		return "Hurl a spike of ice that inflicts 1d8 cold damage and staggers its target.";
	case(SPELL_LIGHTNING_SPEAR):
		return "Fling a penetrating spear of lightning that inflicts 3d8 electric damage.";
	case(SPELL_SLAY_LIVING):
		return "Melee touch attack inflicts 10d6 necrotic damage.";
	case(SPELL_THROW_FIRE):
		return "Hurl fire in a chosen direction. It explodes upon hitting an obstacle, inflicting 1d6 fire damage and igniting everything adjacent.";
	default:
		return "";
	}
}

colorType getSpellColor(const Spell sp)
{
	switch (sp)
	{
	case(SPELL_ARCANE_EMANATION):		return TCODColor::purple;
	case(SPELL_BLOOD_SPEW):				return TCODColor::crimson;
	case(SPELL_CALL_LIGHTNING):			return TCODColor::yellow;
	case(SPELL_CRYSTAL_SPEAR):			return TCODColor::lightPurple;
	case(SPELL_ELECTRIFY):				return TCODColor::darkYellow;
	case(SPELL_EXCRUCIATION):			return TCODColor::red;
	case(SPELL_FIREBALL):				return TCODColor::lightFlame;
	case(SPELL_FRIGID_BLAST):			return TCODColor::lightCyan;
	case(SPELL_ICE_SPIKE):				return TCODColor::cyan;
	case(SPELL_LIGHTNING_SPEAR):		return TCODColor::lightYellow;
	case(SPELL_SLAY_LIVING):			return TCODColor::lightSepia;
	case(SPELL_THROW_FIRE):				return TCODColor::flame;
	default:
		return COLOR_WHITE;
	}
}


//	MP cost of a spell
int getSpellCost(const Spell sp)
{
	switch (sp)
	{
	case(SPELL_ARCANE_EMANATION):
	case(SPELL_CALL_LIGHTNING):
	case(SPELL_ICE_SPIKE):
	case(SPELL_THROW_FIRE):
		return 1;

	case(SPELL_BLOOD_SPEW):
	case(SPELL_ELECTRIFY):
	case(SPELL_FIREBALL):
	case(SPELL_FRIGID_BLAST):
		return 2;

	case(SPELL_CRYSTAL_SPEAR):
	case(SPELL_EXCRUCIATION):
	case(SPELL_LIGHTNING_SPEAR):
	case(SPELL_SLAY_LIVING):
		return 3;

	default:
		return 0;
	}
}


//	If true, we need to aim the spell. Otherwise, it is cast instantly.
bool isTargetedSpell(const Spell sp)
{
	switch (sp)
	{
	case(SPELL_ARCANE_EMANATION):
	case(SPELL_CALL_LIGHTNING):
	case(SPELL_ELECTRIFY):
	case(SPELL_EXCRUCIATION):
	case(SPELL_FIREBALL):
		return false;

	default:
		return true;
	}
}

int getSpellRange(const Spell sp)
{
	switch (sp)
	{
	case(SPELL_SLAY_LIVING):
		return 1;

	case(SPELL_FRIGID_BLAST):
		return 2;

	case(SPELL_ICE_SPIKE):
	case(SPELL_THROW_FIRE):
		return 8;

	case(SPELL_BLOOD_SPEW):
	case(SPELL_CRYSTAL_SPEAR):
	case(SPELL_LIGHTNING_SPEAR):
		return 15;

	default:
		return 0;
	}
}
