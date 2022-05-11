#include "scriptingUtil/gameplaypch.h"
#include "PlayerAnimationManager.h"
#include "PlayerController.h"

Hachiko::Scripting::PlayerAnimationManager::PlayerAnimationManager(GameObject* game_object)
	: Script(game_object, "PlayerAnimationManager")
	, _player_controller(nullptr)
	, _previous_state(PlayerState::INVALID)
	, _state_string("")
	, _animator(nullptr)
	, _idle_index(0)
	, _walking_index(1)
	, _melee_index(2)
	, _ranged_index(3)
	, _dashing_index(4)
{
}

void Hachiko::Scripting::PlayerAnimationManager::OnAwake()
{
	_player_controller = game_object->GetComponent<PlayerController>();
}

void Hachiko::Scripting::PlayerAnimationManager::OnStart()
{
}

void Hachiko::Scripting::PlayerAnimationManager::OnUpdate()
{
	PlayerState current_state = _player_controller->GetState();
	bool state_changed = current_state != _previous_state;
	
	if (!state_changed)
	{
		return;
	}

	_previous_state = current_state;

	switch (current_state)
	{
	case PlayerState::IDLE:
		_state_string = "Play Idle Animation.";
		_animator->StartAnimating(_idle_index, true, 200);
		break;
	case PlayerState::WALKING:
		_state_string = "Play Walking Animation.";
		_animator->StartAnimating(_walking_index, true, 200);
		break;
	case PlayerState::MELEE_ATTACKING:
		_state_string = "Play Melee Attacking Animation.";
		_animator->StartAnimating(_melee_index, true, 200);
		break;
	case PlayerState::RANGED_ATTACKING:
		_state_string = "Play Ranged Attacking Animation.";
		_animator->StartAnimating(_ranged_index, true, 200);
		break;
	case PlayerState::DASHING:
		_state_string = "Play Dashing Animation.";
		_animator->StartAnimating(_dashing_index, true, 200);
		break;
	
	case PlayerState::INVALID:
	default:
		break;
	}
}