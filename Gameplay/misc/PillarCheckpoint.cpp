#include "scriptingUtil/gameplaypch.h"
#include "PillarCheckpoint.h"
#include "entities/player/PlayerController.h"
#include "constants/Scenes.h"

// TODO: This include must go
#include <resources/ResourceAnimation.h>

Hachiko::Scripting::PillarCheckpoint::PillarCheckpoint(GameObject* game_object)
	: Script(game_object, "PillarCheckpoint")
	, _activation_range(5.0f)
	, _respawn_go(nullptr)
{
}

void Hachiko::Scripting::PillarCheckpoint::OnAwake()
{
	_animation = game_object->GetComponent<ComponentAnimation>();
	_restart_position = _respawn_go->GetComponent<ComponentTransform>()->GetGlobalPosition();
	_respawn_go->SetActive(false);
	_animation = game_object->GetComponent<ComponentAnimation>();
	_player = Scenes::GetPlayer();
	_level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
	assert(_player != nullptr);
	assert(_level_manager != nullptr);
}

void Hachiko::Scripting::PillarCheckpoint::OnUpdate()
{
	if (!_player)	return;

	if (!_used && IsPlayerInRange())
	{
		ActivateCheckpoint();
	}

	if (_used && _animation->IsAnimationStopped())
	{
		_animation->SendTrigger("finishedSpinning");
	}
}

void Hachiko::Scripting::PillarCheckpoint::ActivateCheckpoint()
{
	_used = true;

	if (_level_manager)
	{
		_level_manager->SetRespawnPosition(_restart_position);
	}

	// Save Checkpoint on player variable
	_player->GetComponent<PlayerController>()->_initial_pos = _restart_position;

	// Activate animation
	if (!_animation)
	{
		HE_LOG("No animation found. Could not play.");
		return;
	}

	_animation->StartAnimating();
	_animation->SendTrigger("isActive");
}

bool Hachiko::Scripting::PillarCheckpoint::IsPlayerInRange()
{
	float3 player_pos = _player->GetComponent<ComponentTransform>()->GetGlobalPosition();
	float distance_to_player = player_pos.Distance(game_object->GetComponent<ComponentTransform>()->GetGlobalPosition());
	
	return distance_to_player <= _activation_range;
}

