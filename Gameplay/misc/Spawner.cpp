#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/player/CombatManager.h"
#include "misc/Spawner.h"

#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::Spawner::Spawner(GameObject* game_object)
	: Script(game_object, "Spawner")
	, _combat_manager(nullptr)
	, _enemy_pack(nullptr)
	, _player(nullptr)
{
}

void Hachiko::Scripting::Spawner::OnAwake()
{
	if (_combat_manager)
	{
		_combat_manager_script = _combat_manager->GetComponent<CombatManager>();
	}
}

void Hachiko::Scripting::Spawner::OnStart()
{
}

void Hachiko::Scripting::Spawner::OnUpdate()
{
	if (_player && _combat_manager_script && _enemy_pack)
	{
		float distance = Distance(game_object->GetTransform()->GetGlobalPosition(), _player->GetTransform()->GetGlobalPosition());
		if (distance < 5.0f)
		{
			_combat_manager_script->ActivateEnemyPack(_enemy_pack);
			game_object->SetActive(false);
		}
	}
}
