#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/player/CombatManager.h"
#include "misc/Spawner.h"

#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::Spawner::Spawner(GameObject* game_object)
	: Script(game_object, "Spawner")
	, _player(nullptr)
    , _enemy_pack(nullptr)
{
}

void Hachiko::Scripting::Spawner::OnAwake()
{
	_combat_manager_script = Scenes::GetCombatManager()->GetComponent<CombatManager>();
	_player = Scenes::GetPlayer();
	if (_enemy_pack) {
		_enemy_pack->SetActive(false);
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
		if (distance < 3.0f)
		{
			_combat_manager_script->ActivateEnemyPack(_enemy_pack);
			game_object->SetActive(false);
		}
	}
}
