#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/Stats.h"
#include "entities/enemies/BugState.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/PlayerController.h"
#include "constants/Sounds.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

#include <resources/ResourceAnimation.h>
#include <components/ComponentAgent.h>
#include <components/ComponentTransform.h>

#include "misc/AudioManager.h"

Hachiko::Scripting::EnemyController::EnemyController(GameObject* game_object)
	: Script(game_object, "EnemyController")
	, _aggro_range(4)
	, _attack_range(1.5f)
	, _spawn_pos(0.0f, 0.0f, 0.0f)
	, _combat_stats()
	, _spawn_is_initial(false)
	, _player(nullptr)
	, _enemy_body(nullptr)
	, _parasite(nullptr)
	, _state(BugState::INVALID)
	, _attack_animation_duration(0.0f)
	, _attack_animation_timer(0.0f)
	, _is_ranged_attack(false)
	, _audio_manager(nullptr)
	, _audio_manager_game_object(nullptr)
	, _already_in_combat(false)
{
}

void Hachiko::Scripting::EnemyController::OnAwake()
{
	

}

void Hachiko::Scripting::EnemyController::OnStart()
{
	//_attack_range = 1.5f;
	_combat_stats = game_object->GetComponent<Stats>();
	_combat_stats->_attack_power = 1;
	_combat_stats->_attack_cd = _is_ranged_attack ? 2.0f : 1.0f;
	_combat_stats->_move_speed = 4;
	_combat_stats->_max_hp = 2;
	_combat_stats->_current_hp = _combat_stats->_max_hp;
	_stun_time = 0.0f;
	_is_stunned = false;

	_audio_source = game_object->GetComponent<ComponentAudioSource>();

	if (_enemy_body)
	{
		_enemy_body->SetActive(true);
	}
	if (_parasite)
	{
		_parasite->SetActive(false);
	}

	_audio_manager = _audio_manager_game_object->GetComponent<AudioManager>();

	if (_player != nullptr)
	{
		_player_controller = _player->GetComponent<PlayerController>();
	}

	animation = game_object->GetComponent<ComponentAnimation>();

	_acceleration = game_object->GetComponent<ComponentAgent>()->GetMaxAcceleration();
	_speed = game_object->GetComponent<ComponentAgent>()->GetMaxSpeed();
	transform = game_object->GetTransform();
	if (_spawn_is_initial)
	{
		_spawn_pos = transform->GetGlobalPosition();
	}
	animation->StartAnimating();
}

void Hachiko::Scripting::EnemyController::OnUpdate()
{
	CheckState();

	if (!_combat_stats->IsAlive())
	{
		if (_state == BugState::PARASITE)
		{
			if (_current_lifetime >= _parasite_lifespan)
			{
				DestroyEntity();
			}
			else
			{
				_current_lifetime += Time::DeltaTime();
			}
		}
		else
		{
			if (animation != nullptr && animation->IsAnimationStopped())
			{
				DropParasite();
			}
		}
		return;
	}

	_player_pos = _player->GetTransform()->GetGlobalPosition();
	_current_pos = transform->GetGlobalPosition();
	float dist_to_player = _current_pos.Distance(_player_pos);

	if (_is_stunned)
	{
		if (_stun_time > 0.0f)
		{
			_stun_time -= Time::DeltaTime();
			RecieveKnockback();
			return;
		}
		_is_stunned = false;
		ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
		// We set the variables back to normal
		agc->SetMaxAcceleration(_acceleration);
		agc->SetMaxSpeed(_speed);
	}

	// TODO: Delete these after seminar and write a better version.
	if (_state == BugState::ATTACKING)
	{
		_attack_animation_timer += Time::DeltaTime();
		
		if (_attack_animation_timer >= _attack_animation_duration)
		{
			_attack_animation_timer = 0.0f;
			_state = BugState::IDLE;
		}
	}
	else {
		_state = BugState::IDLE;
	}

	if (dist_to_player > _aggro_range)
	{
		GoBack();
	}
	else
	{
		if (dist_to_player <= _attack_range)
		{
			Attack();
		}
		else
		{
			ChasePlayer();
		}
	}
}

BugState Hachiko::Scripting::EnemyController::GetState() const
{
	return _state;
}

const Hachiko::Scripting::Stats* Hachiko::Scripting::EnemyController::GetStats()
{
	return _combat_stats;
}

void Hachiko::Scripting::EnemyController::RegisterHit(int damage, float3 direction, float knockback)
{
	_combat_stats->ReceiveDamage(damage);
	game_object->ChangeColor(float4(255, 255, 255, 255), 0.3f);
	// Knockback
	_is_stunned = true;
	_stun_time = 0.8f; // Once we have weapons stun duration might be moved to each weapon stat
	_knockback_pos = transform->GetGlobalPosition() + (direction * knockback);

	if (!_combat_stats->IsAlive())
	{
		_state = BugState::DEAD;
		animation->SendTrigger("isDead");
	}
}

// Needs to be improved. Player should be able to dodge when enemy starts attacking.
void Hachiko::Scripting::EnemyController::Attack()
{
	if (_state == BugState::DEAD)
	{
		return;
	}
	_attack_cooldown -= Time::DeltaTime();
	_attack_cooldown = _attack_cooldown < 0.0f ? 0.0f : _attack_cooldown;

	if (_is_ranged_attack)
	{
		transform->LookAtTarget(_player->GetTransform()->GetGlobalPosition());

		// Make the enemy stop (quick fix)
		ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
		agc->SetTargetPosition(game_object->GetTransform()->GetGlobalPosition());
	}

	if (_attack_cooldown > 0.0f)
	{
		return;
	}

	_state = BugState::ATTACKING;
	
	if (_is_ranged_attack) 
	{
		math::float3 forward = _player->GetTransform()->GetGlobalPosition() - game_object->GetTransform()->GetGlobalPosition();
		forward = forward.Normalized();
		// Removed bullet
	}
	else
	{
		_player_controller->RegisterHit(_combat_stats->_attack_power);
	}
	_attack_cooldown = _combat_stats->_attack_cd;
}

void Hachiko::Scripting::EnemyController::ChasePlayer()
{
	if (_state == BugState::DEAD)
	{
		return;
	}

	_state = BugState::MOVING;

	float3 corrected_pos = Navigation::GetCorrectedPosition(_player_pos, math::float3(10.0f, 10.0f, 10.0f));
	if (corrected_pos.x < FLT_MAX)
	{
		_target_pos = corrected_pos;
		transform->LookAtTarget(_target_pos);
		MoveInNavmesh();
	}
}

void Hachiko::Scripting::EnemyController::GoBack()
{
	if (_state == BugState::DEAD)
	{
		return;
	}

	_state = BugState::MOVING_BACK;

	float3 corrected_pos = Navigation::GetCorrectedPosition(_spawn_pos, math::float3(10.0f, 10.0f, 10.0f));
	if (corrected_pos.x < FLT_MAX)
	{
		_target_pos = corrected_pos;
		transform->LookAtTarget(_target_pos);
		MoveInNavmesh();
	}
}

void Hachiko::Scripting::EnemyController::Stop()
{
	_target_pos = transform->GetGlobalPosition();
	MoveInNavmesh();
}

void Hachiko::Scripting::EnemyController::RecieveKnockback()
{
	ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
	_target_pos = Navigation::GetCorrectedPosition(_knockback_pos, math::float3(10.0f, 1.0f, 10.0f));
	// We exagerate the movement
	agc->SetMaxAcceleration(50.0f);
	agc->SetMaxSpeed(30.0f);
	agc->SetTargetPosition(_target_pos);
}

void Hachiko::Scripting::EnemyController::MoveInNavmesh()
{
	ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
	agc->SetTargetPosition(_target_pos);
}

void Hachiko::Scripting::EnemyController::DestroyEntity()
{
	game_object->SetActive(false);
	SceneManagement::Destroy(game_object);
}

void Hachiko::Scripting::EnemyController::DropParasite()
{
	Stop();
	_state = BugState::PARASITE;
	//TODO: Check if in scene there's already a parasite? Maybe?
	if (_enemy_body) {
		_enemy_body->SetActive(false);
	}

	if (_parasite) {
		_parasite->SetActive(true);
	}
}

void Hachiko::Scripting::EnemyController::GetParasite()
{
	DestroyEntity();
}

void Hachiko::Scripting::EnemyController::CheckState()
{
	BugState current_state = GetState();
	bool state_changed = current_state != _previous_state;

	if (!state_changed)
	{
		return;
	}

	if ((_previous_state == BugState::ATTACKING || _previous_state == BugState::MOVING) && 
		(current_state == BugState::IDLE || current_state == BugState::MOVING_BACK || current_state == BugState::DEAD))
	{
		if (_already_in_combat)
		{
			_audio_manager->UnregisterCombat();
			_already_in_combat = false;
		}
	} 
	else if ((current_state == BugState::ATTACKING || current_state == BugState::MOVING) &&
		     (_previous_state == BugState::IDLE || _previous_state == BugState::MOVING_BACK || _previous_state == BugState::DEAD))
	{
		if (!_already_in_combat)
		{
			_audio_manager->RegisterCombat();
			_already_in_combat = true;
		}
	}

	_previous_state = current_state;

	switch (current_state)
	{
	case BugState::IDLE:
		animation->SendTrigger("idle");
		break;
	case BugState::ATTACKING:
		_audio_source->PostEvent(Sounds::ENEMY_ATTACK);
		animation->SendTrigger("isAttacking");
		break;
	case BugState::DEAD:
		_audio_source->PostEvent(Sounds::ENEMY_DIE);
		animation->SendTrigger("isDead");
		break;
	case BugState::MOVING:
		animation->SendTrigger("isMoving");
		break;
	case BugState::MOVING_BACK:
		animation->SendTrigger("isMoving");
		break;

	case BugState::INVALID:
	default:
		break;
	}
}