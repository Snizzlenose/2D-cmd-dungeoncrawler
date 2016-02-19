#pragma once

#include "BattleSystem.h"
#include <vector>
#include <string>
#include <map>
#include <intrin.h>

struct Spell
{
	Spell( const std::string& name, double damageMultiplier );

	const std::string name;
	const double damageMultiplier;
};

struct Combatant
{
	Combatant( const std::string& name, int health, int healthMax, int healthRegeneration, int damage, int spells = 0 );

	std::string name;
	int health;
	int healthMax;
	int healthRegeneration;
	int damage;
	int spells;

	void Update( );
};

class BattleSystem
{
	public:
		BattleSystem( );

		Combatant GetRandomMonster( );
		Spell GetSpell( Combatant& player );
		void CastSpell( const Spell& spell, Combatant& caster, Combatant& target );
		void WeaponAttack( Combatant& attacker, Combatant& target );
		void PlayerTurn( Combatant& player, Combatant& monster );
		void MonsterTurn( Combatant& player, Combatant& monster );
		void EngageRandomMonster( Combatant& player );

	private:
		const std::vector<Combatant> _libraryMonster;
		const std::map<int, const Spell> _librarySpell;
};