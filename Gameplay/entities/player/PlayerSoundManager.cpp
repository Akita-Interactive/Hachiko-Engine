#include "scriptingUtil/gameplaypch.h"
#include "constants/Sounds.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"

Hachiko::Scripting::PlayerSoundManager::PlayerSoundManager(Hachiko::GameObject* game_object)
	: Script(game_object, "PlayerSoundManager")
	, _player_controller(nullptr)
	, _timer(0.0f)
	, _step_frequency(0.0f)
	, _melee_frequency(0.0f)
	, _ranged_frequency(0.0f)
	, _previous_state(PlayerState::INVALID)
	, _audio_source(nullptr)
{
}

void Hachiko::Scripting::PlayerSoundManager::OnAwake()
{
	_player_controller = game_object->GetComponent<PlayerController>();
}

void Hachiko::Scripting::PlayerSoundManager::OnUpdate()
{
	PlayerState state = _player_controller->GetState();

	bool state_changed = _previous_state != state;
	_previous_state = state;

	float delta_time = Time::DeltaTime();

	if (state_changed)
	{
		_timer = 0.0f;
	}

	switch (state)
	{
	case PlayerState::IDLE:
		_timer = 0.0f;
		break;

	case PlayerState::WALKING:
		_current_frequency = _step_frequency;

		if (_timer == 0.0f)
		{
			_audio_source->PostEvent(Sounds::FOOTSTEP);
		}

		break;

	case PlayerState::MELEE_ATTACKING:
		_current_frequency = _melee_frequency;

		if (_timer == 0.0f)
		{
			_audio_source->PostEvent(Sounds::MELEE_ATTACK);
		}

		break;
	case PlayerState::RANGED_ATTACKING:
		_current_frequency = _ranged_frequency;

		if (_timer == 0.0f)
		{
			_audio_source->PostEvent(Sounds::RANGED_ATTACK);
		}

		break;
	case PlayerState::DASHING:
		if (state_changed)
		{
			_audio_source->PostEvent(Sounds::DASH);
		}
		_timer = 0.0f;
		break;
	case PlayerState::DIE:
		if (state_changed)
		{
			_audio_source->PostEvent(Sounds::GAME_OVER);
			_audio_source->PostEvent(Sounds::STOP_COMBAT);
			_audio_source->PostEvent(Sounds::STOP_PEBBLE);
			_audio_source->PostEvent(Sounds::STOP_WIND);
		}
		_timer = 0.0f;
		break;
	case PlayerState::PICK_UP:
		if (state_changed)
		{
			_audio_source->PostEvent(Sounds::PARASITE_PICKUP);
		}
		_timer = 0.0f;
		break;
	case PlayerState::INVALID:
	default:
		_timer = 0.0f;
		break;
	}

	_timer += delta_time;

	if (_timer >= _step_frequency)
	{
		_timer = 0.0f;
	}
}