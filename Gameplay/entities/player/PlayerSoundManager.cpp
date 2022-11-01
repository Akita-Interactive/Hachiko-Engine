#include "scriptingUtil/gameplaypch.h"
#include "constants/Sounds.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"

Hachiko::Scripting::PlayerSoundManager::PlayerSoundManager(Hachiko::GameObject* game_object)
	: Script(game_object, "PlayerSoundManager")
	, _player_controller(nullptr)
	, _timer(0.0f)
	, _damage_timer(0.0f)
	, _step_frequency(0.0f)
	, _ranged_frequency(0.0f)
	, _dmg_cool_down(1.0f)
	, _previous_state(PlayerState::INVALID)
	, _audio_source(nullptr)
{
}

void Hachiko::Scripting::PlayerSoundManager::OnAwake()
{
	_player_controller = game_object->GetComponent<PlayerController>();
}

void Hachiko::Scripting::PlayerSoundManager::OnStart()
{
	if (_audio_source == nullptr)
	{
		return;
	}

	SetGroundEffect();
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
		if (_player_controller->IsAttackSoundRequested())
		{
			_audio_source->PostEvent(Sounds::MELEE_ATTACK);
			_player_controller->AttackSoundPlayed();
		}

		break;

	case PlayerState::RANGED_CHARGING:
		_current_frequency = 1.0f;// _ranged_frequency;

		if (_timer == 0.0f)
		{
			_audio_source->PostEvent(Sounds::LOADING_AMMO);
		}

		break;

	case PlayerState::RANGED_ATTACKING:

		if (state_changed)
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
			_audio_source->PostEvent(Sounds::PLAY_DEAD_MUSIC);
			_audio_source->PostEvent(Sounds::STOP_PEBBLE);
			_audio_source->PostEvent(Sounds::STOP_WIND);

			if (_level == 3)
			{
				_audio_source->PostEvent(Sounds::SET_STATE_DEFEATED); // Boss fight
			}
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

	PlayerController::DamageType damage = _player_controller->ReadDamageState();
	
	if (_damage_timer >= _dmg_cool_down &&
		state != PlayerState::DIE && 
		state != PlayerState::READY_TO_RESPAWN)
	{
		
		if (damage == PlayerController::DamageType::ENEMY ||
			damage == PlayerController::DamageType::LASER ||
			damage == PlayerController::DamageType::CRYSTAL)
		{
			_audio_source->PostEvent(Sounds::RECEIVE_DAMAGE);
			_damage_timer = 0.0f;
		}
	}

	_timer += delta_time;
	_damage_timer += delta_time;

	if (_timer >= _current_frequency)
	{
		_timer = 0.0f;
	}
}

void Hachiko::Scripting::PlayerSoundManager::SetGroundEffect()
{
	if (_level == 1)
	{
		_audio_source->SetSwitch(Sounds::SWITCH_GROUP_FOOTSTEPS, Sounds::SWITCH_STATE_FOOTSTEPS_GRAVEL);
	}
	else
	{
		_audio_source->SetSwitch(Sounds::SWITCH_GROUP_FOOTSTEPS, Sounds::SWITCH_STATE_FOOTSTEPS_STANDARD);
	}
}
