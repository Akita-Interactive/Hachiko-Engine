#include "scriptingUtil/gameplaypch.h"

#include "LevelManager.h"

#include "entities/player/PlayerController.h"

Hachiko::Scripting::LevelManager::LevelManager(GameObject* game_object)
	: Script(game_object, "LevelManager")
	, _respawn_position(float3::zero)
	, _last_gauntlet(nullptr)
{}

void Hachiko::Scripting::LevelManager::Respawn(PlayerController * player)
{
	if (_last_gauntlet != nullptr && !_last_gauntlet->IsCompleted())
	{
		_last_gauntlet->ResetGauntlet();
	}

	player->ResetPlayer(GetRespawnPosition());
}
