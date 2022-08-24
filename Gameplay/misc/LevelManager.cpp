#include "scriptingUtil/gameplaypch.h"

#include "LevelManager.h"

#include "entities/player/PlayerController.h"

Hachiko::Scripting::LevelManager::LevelManager(GameObject* game_object)
	: Script(game_object, "LevelManager")
	, _respawn_position(float3::zero)
	, _last_gauntlet(nullptr)
{}

void Hachiko::Scripting::LevelManager::OnAwake()
{
	_enemy_counter = _gauntlet_counter_go->GetComponent<ComponentText>();
	_gauntlet_ui_go->SetActive(false);
}

void Hachiko::Scripting::LevelManager::OnUpdate()
{
	if (!_last_gauntlet) return;
	_gauntlet_ui_go->SetActive(_last_gauntlet && !_last_gauntlet->IsCompleted());
}

void Hachiko::Scripting::LevelManager::SetGauntlet(GauntletManager* last_gauntlet)
{
	_last_gauntlet = last_gauntlet;
}

void Hachiko::Scripting::LevelManager::SetEnemyCount(unsigned count)
{
	_enemy_counter->SetText(std::to_string(count).c_str());
}

float3 Hachiko::Scripting::LevelManager::Respawn()
{
	if (_last_gauntlet != nullptr && !_last_gauntlet->IsCompleted())
	{
		_last_gauntlet->ResetGauntlet();
	}

	return GetRespawnPosition();

	//Disable gauntlet ui
}
