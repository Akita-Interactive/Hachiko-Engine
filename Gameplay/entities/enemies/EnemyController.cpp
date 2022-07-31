#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/Stats.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerCamera.h"
#include "constants/Sounds.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

#include <resources/ResourceAnimation.h>
#include <components/ComponentAgent.h>
#include <components/ComponentTransform.h>
#include "constants/Scenes.h"

#include "entities/player/CombatVisualEffectsPool.h"
#include "misc/AudioManager.h"

Hachiko::Scripting::EnemyController::EnemyController(GameObject* game_object)
	: Script(game_object, "EnemyController")
	, _aggro_range(4)
	, _attack_range(3.0f)
	, _attack_delay(0.3f)
	, _idle_cooldown(2.0f)
	, _spawn_pos(0.0f, 0.0f, 0.0f)
	, _combat_stats()
	, _spawn_is_initial(false)
	, _enemy_body(nullptr)
	, _parasite(nullptr)
	, _state(BugState::INVALID)
	, _attack_animation_duration(0.0f)
	, _attack_animation_timer(0.0f)
	, _audio_manager(nullptr)
	, _already_in_combat(false)
	, _is_from_gautlet(false)
	, _combat_visual_effects_pool(nullptr)
	, _worm(false)
	, _small_dust(nullptr)
	, _big_dust(nullptr)
	, _spawning_time(2.0f)
	, _will_die(true)
	, _attack_zone(nullptr)
	, _inner_indicator(nullptr)
	, _outer_indicator(nullptr)
{
	// Push attack
	push_attack.damage = 0;
	push_attack.knockback_distance = 5.0f;
	push_attack.width = 0.f;
	push_attack.range = 5.0f; // a bit bigger than its attack activation range
	push_attack.type = CombatManager::AttackType::CIRCLE;
}

void Hachiko::Scripting::EnemyController::OnAwake()
{	
	if (_worm)
	{
		_enemy_type = EnemyType::WORM;
	}
	else
	{
		_enemy_type = EnemyType::BEETLE;
	}
	
	_player = Scenes::GetPlayer();
	_audio_manager_game_object = Scenes::GetAudioManager();
	_combat_manager = Scenes::GetCombatManager()->GetComponent<CombatManager>();
	_player_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
	_combat_visual_effects_pool = 
		Scenes::GetCombatVisualEffectsPool()->GetComponent<CombatVisualEffectsPool>();

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		_combat_stats = game_object->GetComponent<Stats>();
		_combat_stats->_attack_power = 1;
		_combat_stats->_current_hp = _combat_stats->_max_hp;
		_stun_time = 0.0f;
		_is_stunned = false;
		_attack_delay = 0.3f;
		_audio_source = game_object->GetComponent<ComponentAudioSource>();
		_audio_manager = _audio_manager_game_object->GetComponent<AudioManager>();

		if (_blood_trail != nullptr)
		{
			_blood_trail_particles = _blood_trail->GetComponent<ComponentParticleSystem>();
		}
		_acceleration = game_object->GetComponent<ComponentAgent>()->GetMaxAcceleration();
		_speed = game_object->GetComponent<ComponentAgent>()->GetMaxSpeed();
		break;

	case EnemyType::WORM:
		_combat_stats = game_object->GetComponent<Stats>();
		_combat_stats->_attack_power = 1;
		_combat_stats->_current_hp = _combat_stats->_max_hp;
		_stun_time = 0.0f;
		_is_stunned = false;
		_attack_delay = 0.3f;
		_audio_source = game_object->GetComponent<ComponentAudioSource>();
		_audio_manager = _audio_manager_game_object->GetComponent<AudioManager>();
		
		if (_blood_trail != nullptr)
		{
			_blood_trail_particles = _blood_trail->GetComponent<ComponentParticleSystem>();
		}

		if (_small_dust)
		{
			_small_dust_particles = _small_dust->GetComponent<ComponentParticleSystem>();
			if (_small_dust_particles)
			{
				_small_dust_particles->Stop();
			}
		}
		if (_big_dust)
		{
			_big_dust_particles = _big_dust->GetComponent<ComponentParticleSystem>();
			if (_big_dust_particles)
			{
				_big_dust_particles->Stop();
			}
		}
		if (_inner_indicator)
		{
			_inner_indicator_billboard = _inner_indicator->GetComponent<ComponentBillboard>();
			if (_inner_indicator_billboard)
			{
				_inner_indicator_billboard->Stop();
			}
		}
		if (_outer_indicator)
		{
			_outer_indicator_billboard = _outer_indicator->GetComponent<ComponentBillboard>();
			if (_outer_indicator_billboard)
			{
				_outer_indicator_billboard->Stop();
			}
		}
		break;
	}

	animation = game_object->GetComponent<ComponentAnimation>();

	transform = game_object->GetTransform();

	_player = Scenes::GetPlayer();
	if (_player != nullptr)
	{
		_player_controller = _player->GetComponent<PlayerController>();
	}

	if (_spawn_is_initial)
	{
		_spawn_pos = transform->GetGlobalPosition();
		_spawn_rot = transform->GetGlobalRotation();
	}

	animation->StartAnimating();

	srand((unsigned)time(NULL));
}

void Hachiko::Scripting::EnemyController::OnStart()
{
	if (_enemy_body != nullptr)
	{
		_enemy_body->SetActive(true);
	}
	if (_parasite != nullptr)
	{
		_parasite->SetActive(false);
	}

	ResetEnemy();

	if (!_has_spawned)
	{
		Spawn();
	}
}

void Hachiko::Scripting::EnemyController::OnUpdate()
{
	CheckState();

	if (_state == BugState::INVALID && !_has_spawned)
	{
		Spawn();
		return;
	}

	if (_current_spawning_time > 0.0f || _state == BugState::SPAWNING)
	{
		SpawnController();
		return;
	}

	if (!_combat_stats->IsAlive())
	{
		DeathController();
		return;
	}

	StunController();

	if (_is_stunned)
	{
		return;
	}

	_player_pos = _player_controller->GetGameObject()->GetTransform()->GetGlobalPosition();
	_current_pos = transform->GetGlobalPosition();

	IdleController();

	AttackController();

}

Hachiko::Scripting::BugState Hachiko::Scripting::EnemyController::GetState() const
{
	return _state;
}

const Hachiko::Scripting::Stats* Hachiko::Scripting::EnemyController::GetStats()
{
	return _combat_stats;
}

void Hachiko::Scripting::EnemyController::SpawnController()
{
	

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		// Beetle logic may change once we have its spawning animations
		_current_spawning_time -= Time::DeltaTime();
		if (_current_spawning_time <= 0.0f)
		{
			_state = BugState::IDLE;
		}
		break;

	case EnemyType::WORM:
		if (_enemy_body->IsActive() && _state != BugState::SPAWNING)
		{
			return;
		}

		if (_enemy_body->IsActive() && _state == BugState::SPAWNING && animation->IsAnimationStopped())
		{
			_state = BugState::IDLE;
			return;
		}

		if (_state == BugState::SPAWNING && _enemy_body)
		{
			_player_camera->Shake(0.5f, 0.8f);
			_enemy_body->SetActive(true);
			_big_dust_particles->Restart();
			//Push the player back
			_combat_manager->EnemyMeleeAttack(transform->GetGlobalMatrix(), push_attack);
			return;
		}

		_current_spawning_time -= Time::DeltaTime();
		if (_current_spawning_time <= 0.0f)
		{
			_state = BugState::SPAWNING;
			animation->ResetState();
		}
		break;
	}
}

void Hachiko::Scripting::EnemyController::DeathController()
{
	float alpha_transition;
	switch (_state)
	{
	case BugState::PARASITE:

		_enemy_body->SetActive(false);
		_parasite_dissolving_time_progress += Time::DeltaTime();
		alpha_transition = math::Sqrt(_parasite_dissolve_time - _parasite_dissolving_time_progress) * _parasite_dissolving;
		_parasite->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, alpha_transition), true);

		if (_parasite_dissolving_time_progress >= _parasite_dissolve_time)
		{
			DestroyEntity();
		}
		break;
	case BugState::HIDEN:
		if (animation->IsAnimationStopped())
		{
			DestroyEntity();
		}
		break;
	case BugState::DEAD:
		if (animation->IsAnimationStopped())
		{
			if (_enemy_dissolve_time >= _enemy_dissolving_time_progress)
			{
				_enemy_dissolving_time_progress += Time::DeltaTime();
				alpha_transition = math::Sqrt(_enemy_dissolve_time - _enemy_dissolving_time_progress) * _enemy_dissolving;
				_enemy_body->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, alpha_transition), true);
			}
			else
			{
				DropParasite();
			}
		}
		break;
	default:
		if (!_will_die && _enemy_type == EnemyType::WORM)
		{
			_audio_manager->UnregisterCombat();
			_state = BugState::HIDEN;
			game_object->GetComponent<ComponentAgent>()->RemoveFromCrowd();
			return;
		}
		else
		{
			if (_enemy_type == EnemyType::WORM)
			{
				_audio_manager->UnregisterCombat();
			}
			_state = BugState::DEAD;
			game_object->GetComponent<ComponentAgent>()->RemoveFromCrowd();
			return;
		}
	}
}

void Hachiko::Scripting::EnemyController::StunController()
{
	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
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
		break;
	case EnemyType::WORM:
		if (_state == BugState::HIT)
		{
			_inmune = true;
			_is_stunned = true;
		}
		else
		{
			_inmune = false;
			_is_stunned = false;
		}

		if (_previous_state == BugState::HIT)
		{
			if (animation->IsAnimationStopped())
			{
				_state = BugState::IDLE;
			}
		}
		break;
	}

}

void Hachiko::Scripting::EnemyController::AttackController()
{
	float dist_to_player = _current_pos.Distance(_player_pos);

	_attack_cooldown -= Time::DeltaTime();
	_attack_cooldown = _attack_cooldown < 0.0f ? 0.0f : _attack_cooldown;

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:

		_enraged -= Time::DeltaTime();
		_enraged = _enraged < 0.0f ? 0.0f : _enraged;

		if (IsAttacking())
		{
			_attack_animation_timer += Time::DeltaTime();

			if (_attack_animation_timer >= _attack_animation_duration)
			{
				_attack_animation_timer = 0.0f;
				_state = BugState::IDLE;
				_current_idle_cooldown = _idle_cooldown;
			}
		}
		else {
			_state = BugState::IDLE;
		}

		// If an enemy is from a gautlet, it will always follow the player
		if (_is_from_gautlet || dist_to_player < _aggro_range || _enraged > 0.0f)
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
		break;
	case EnemyType::WORM:
		if (_attack_cooldown > 0.0f)
		{
			return;
		}

		if (!_attack_landing)
		{
			_attack_zone->GetTransform()->SetGlobalPosition(_player_pos);
			WormSpit();
			_attack_animation_timer = 0.0f;
		}
		else
		{
			_attack_animation_timer += Time::DeltaTime();
			if (_attack_animation_timer >= 1.0f)
			{
				CombatManager::AttackStats attack_stats;
				attack_stats.damage = _combat_stats->_attack_power;
				attack_stats.knockback_distance = 0.0f;
				attack_stats.width = 0.f;
				attack_stats.range = 2.5; // a bit bigger than its attack activation range
				attack_stats.type = CombatManager::AttackType::CIRCLE;
				_combat_manager->EnemyMeleeAttack(_attack_zone->GetTransform()->GetGlobalMatrix(), attack_stats);
				_attack_landing = false;
				_attack_cooldown = _combat_stats->_attack_cd;
				_state = BugState::IDLE;
			}
		}
		return;
	}
}

void Hachiko::Scripting::EnemyController::IdleController()
{
	if (_state == BugState::ATTACKING && animation->IsAnimationStopped())
	{
		_state = BugState::IDLE;
	}

	if (_enemy_type == EnemyType::WORM)
	{
		_enemy_body->GetTransform()->LookAtTarget(_player_pos);
		return;
	}

	if (_state == BugState::IDLE)
	{
		_current_idle_cooldown -= Time::DeltaTime();
		if (_current_idle_cooldown <= 0.0f)
		{
			PatrolMovement();
		}
	}

	if (_state == BugState::PATROL)
	{
		if (_current_pos.Distance(_target_pos) > 0.3f)
		{
			_state = BugState::IDLE;
			_current_idle_cooldown = _idle_cooldown;
		}
	}
}

void Hachiko::Scripting::EnemyController::RegisterHit(int damage, float3 direction, float knockback, bool is_from_player, bool is_ranged)
{
	if (_state == BugState::SPAWNING || !_enemy_body->IsActive() || _inmune)
	{
		return; // Inmune while spawning or if it isn't there
	}

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		_knockback_pos = transform->GetGlobalPosition() + (direction * knockback);
		break;
	case EnemyType::WORM:
		_knockback_pos = transform->GetGlobalPosition();
		_state = BugState::HIT;
		break;
	}

	if (is_from_player)
	{
		_enraged = 5.0f;

		// TODO: Trigger this via an event of player, that is subscribed by
		// combat visual effects pool.
		if (!is_ranged)
		{
			_combat_visual_effects_pool->PlayPlayerAttackEffect(
				_player_controller->GetCurrentWeaponType(),
				_player_controller->GetAttackIndex(),
				game_object->GetTransform()->GetGlobalPosition());
		}
	}

	if (_blood_trail_particles != nullptr)
	{
		_blood_trail_particles->Enable();
		_blood_trail_particles->Restart();
	}

	_combat_stats->ReceiveDamage(damage);
	game_object->ChangeEmissiveColor(float4(255, 255, 255, 255), 0.3f, true);
	// Knockback
	_is_stunned = true;
	_stun_time = 0.8f; // Once we have weapons stun duration might be moved to each weapon stat
}

// Needs to be improved. Player should be able to dodge when enemy starts attacking.
void Hachiko::Scripting::EnemyController::Attack()
{
	if (_state == BugState::DEAD || _attack_cooldown > 0.0f)
	{
		return;
	}

	if (_attack_current_delay <= 0.0f || _previous_state != BugState::ATTACKING)
	{
		_attack_current_delay = _attack_delay;
		_state = BugState::ATTACKING;
	}

	_attack_current_delay -= Time::DeltaTime();

	if (_attack_current_delay > 0.0f)
	{
		return;
	}

	CombatManager::AttackStats attack_stats;

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		attack_stats.damage = _combat_stats->_attack_power;
		attack_stats.knockback_distance = 0.0f;
		attack_stats.width = 4.f;
		attack_stats.range = _attack_range * 1.1f; // a bit bigger than its attack activation range
		attack_stats.type = CombatManager::AttackType::RECTANGLE;

		Debug::DebugDraw(_combat_manager->CreateAttackHitbox(GetMeleeAttackOrigin(attack_stats.range), attack_stats), float3(1.0f, 1.0f, 0.0f));

		_combat_manager->EnemyMeleeAttack(GetMeleeAttackOrigin(attack_stats.range), attack_stats);

		transform->LookAtTarget(_player_pos);
		Stop();

		break;

	case EnemyType::WORM:
		break;
	}

	_attack_cooldown = _combat_stats->_attack_cd;
}


void Hachiko::Scripting::EnemyController::DestroyEntity()
{
	game_object->SetActive(false);
	//SceneManagement::Destroy(game_object);
}

void Hachiko::Scripting::EnemyController::DropParasite()
{
	Stop();
	_state = BugState::PARASITE;
	//TODO: Check if in scene there's already a parasite? Maybe?
	if (_parasite) {
		_parasite->SetActive(true);
	}

	_parasite_dropped = true;
}

void Hachiko::Scripting::EnemyController::GetParasite()
{
	DestroyEntity();
}

void Hachiko::Scripting::EnemyController::Spawn()
{
	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		_current_spawning_time = _spawning_time;
		if (_enemy_body)
		{
			_enemy_body->SetActive(true);
			_enemy_body->ChangeEmissiveColor(float4(0.3f, 0.5f, 1.0f, 1.0f), _spawning_time, true);
		}
		if (_parasite)
		{
			_parasite->SetActive(false);
		}
		_has_spawned = true;
		_state = BugState::SPAWNING;
		break;

	case EnemyType::WORM:
		_has_spawned = true;
		if (_enemy_body)
		{
			_enemy_body->SetActive(false);
		}
		if (_parasite)
		{
			_parasite->SetActive(false);
		}
		_state = BugState::INVALID;
		_small_dust_particles->Restart();
		_current_spawning_time = _spawning_time;
		_player_camera->Shake(_spawning_time, 0.8f);
		break;
	}
}

void Hachiko::Scripting::EnemyController::CheckState()
{
	BugState current_state = GetState();
	bool state_changed = current_state != _previous_state;

	if (!state_changed)
	{
		return;
	}

	if (((_previous_state == BugState::ATTACKING || _previous_state == BugState::MOVING) && 
		(current_state == BugState::IDLE || current_state == BugState::MOVING_BACK || current_state == BugState::DEAD))
		|| (_enemy_type == EnemyType::WORM && (current_state == BugState::DEAD || (current_state == BugState::HIDEN))))
	{
		if (_already_in_combat)
		{
			_audio_manager->UnregisterCombat();
			_already_in_combat = false;
		}
	} 
	else if (((current_state == BugState::ATTACKING || current_state == BugState::MOVING) &&
		     (_previous_state == BugState::IDLE || _previous_state == BugState::MOVING_BACK || _previous_state == BugState::DEAD))
		|| (_enemy_type == EnemyType::WORM && current_state != BugState::DEAD && current_state != BugState::HIDEN))
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
	case BugState::PATROL:
		animation->SendTrigger("idle");
		break;
	case BugState::HIT:
		animation->SendTrigger("isHit");
		break;
	case BugState::HIDEN:
		animation->SendTrigger("isHide");
		break;
	case BugState::SPAWNING:
		animation->SendTrigger("isAppear");
		break;
	case BugState::INVALID:
	default:
		break;
	}
}

void Hachiko::Scripting::EnemyController::ResetEnemy()
{
	_combat_stats->_current_hp = _combat_stats->_max_hp;
	_stun_time = 0.0f;
	_is_stunned = false;
	_has_spawned = false;
	_attack_delay = 0.3f;
	_state = BugState::INVALID;
	_previous_state = BugState::INVALID;
	_parasite_dissolving_time_progress = 0.f;
	_enemy_dissolving_time_progress = 0.f;
	_parasite_dropped = false;

	if (_enemy_body)
	{
		_enemy_body->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, 1.0f), true);
	}
	
	if (_parasite)
	{
		_parasite->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, 1.0f), true);
		_parasite->SetActive(false);
	}

	if (_blood_trail_particles != nullptr)
	{
		_blood_trail_particles->Disable();
	}

	if (_inner_indicator_billboard != nullptr)
	{
		_inner_indicator_billboard->Stop();
	}

	if (_outer_indicator_billboard != nullptr)
	{
		_outer_indicator_billboard->Stop();
	}

	if (_big_dust_particles != nullptr)
	{
		_big_dust_particles->Stop();
	}

	if (_small_dust_particles != nullptr)
	{
		_small_dust_particles->Stop();
	}
}

void Hachiko::Scripting::EnemyController::ResetEnemyPosition()
{
	transform->SetGlobalPosition(_spawn_pos);
	transform->SetGlobalRotation(_spawn_rot);
}

float4x4 Hachiko::Scripting::EnemyController::GetMeleeAttackOrigin(float attack_range) const
{
	ComponentTransform* enemy_transform = game_object->GetTransform();
	float3 emitter_direction = enemy_transform->GetFront().Normalized();
	float3 emitter_position = _current_pos + emitter_direction * (attack_range / 2.f);
	float4x4 emitter = float4x4::FromTRS(emitter_position, enemy_transform->GetGlobalRotation(), enemy_transform->GetGlobalScale());
	return emitter;
}

/*
* 
* BEETLE UNIQUE FUNCTIONS
* 
*/


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
		_speed = _combat_stats->_move_speed;
		ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
		agc->SetMaxSpeed(_speed);
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

void Hachiko::Scripting::EnemyController::PatrolMovement()
{
	_state = BugState::PATROL;

	float random = ((float(rand()) / float(RAND_MAX)) * (5.0f - -5.0f)) + -5.0f;

	float3 _new_pos = float3(_spawn_pos.x + random, _spawn_pos.y, _spawn_pos.z + random);

	float3 corrected_pos = Navigation::GetCorrectedPosition(_new_pos, math::float3(10.0f, 10.0f, 10.0f));
	if (corrected_pos.x < FLT_MAX)
	{
		_target_pos = corrected_pos;
		transform->LookAtTarget(_target_pos);
		_speed = 3.0f;
		ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
		agc->SetMaxSpeed(_speed);
		MoveInNavmesh();
	}
}

/*
*
* WORM UNIQUE FUNCTIONS
*
*/

void Hachiko::Scripting::EnemyController::WormSpit()
{
	_state = BugState::ATTACKING;
	_attack_landing = true;
	_attack_current_delay = 1.0f;
	_inner_indicator_billboard->Play();
	_outer_indicator_billboard->Play();
}