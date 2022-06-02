#include "scriptingUtil/gameplaypch.h"
#include <Stats.h>

Hachiko::Scripting::Stats::Stats(GameObject* game_object)
	: Script(game_object, "Stats")
	, _attack_power(1)
	, _attack_cd(2)
	, _attack_range(3.5f)
	, _move_speed(7.0f)
	, _max_hp(3)
	, _current_hp(_max_hp)
{
}

bool Hachiko::Scripting::Stats::IsAlive()
{
	return _current_hp > 0;
}

void Hachiko::Scripting::Stats::ReceiveDamage(int damage)
{
	_current_hp -= damage;
	math::Clamp(_current_hp, 0, _max_hp);
}