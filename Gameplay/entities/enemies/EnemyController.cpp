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
	, _enemy_body(nullptr)
	, _parasite(nullptr)
	, _state(EnemyState::INVALID)
	, _attack_animation_duration(0.0f)
	, _attack_animation_timer(0.0f)
	, _audio_manager(nullptr)
	, _already_in_combat(false)
	, _is_from_gautlet(false)
	, _is_from_boss(false)
	, _combat_visual_effects_pool(nullptr)
	, _worm(false)
	, _small_dust(nullptr)
	, _big_dust(nullptr)
	, _spawning_time(2.0f)
	, _will_die(true)
	, _attack_zone(nullptr)
	, _inner_indicator(nullptr)
	, _outer_indicator(nullptr)
	, _projectile_particles(nullptr)
	, _explosion_particles(nullptr)
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

	// Mark enemies as using scaled delta time for its components such as agent,
	// particle, animation and billboard to respond to the changes in time
	// scale:
	game_object->SetTimeScaleMode(TimeScaleMode::SCALED);

	GetComponents();

	SetStats();

	SetVfx();

	_spawn_pos = transform->GetGlobalPosition();
	_spawn_rot = transform->GetGlobalRotation();

	if (animation)
	{
		animation->StartAnimating();
	}

	_enemy_body->ChangeDissolveProgress(0.0f, true);

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

	if (_state == EnemyState::INVALID && !_has_spawned)
	{
		Spawn();
		return;
	}

	if (_current_spawning_time > 0.0f || _state == EnemyState::SPAWNING)
	{
		spawn_progress += spawn_rate * Time::DeltaTimeScaled();
		
		_enemy_body->ChangeDissolveProgress(spawn_progress, true);

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

	MovementController();
	AttackController();
}

void Hachiko::Scripting::EnemyController::SpawnController()
{
	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		// Beetle logic may change once we have its spawning animations
		_current_spawning_time -= Time::DeltaTimeScaled();
		if (_current_spawning_time <= 0.0f)
		{
			_state = EnemyState::IDLE;
		}
		break;

	case EnemyType::WORM:
		if (_enemy_body->IsActive() && _state != EnemyState::SPAWNING)
		{
			return;
		}

		if (_enemy_body->IsActive() && _state == EnemyState::SPAWNING && animation->IsAnimationStopped())
		{
			_state = EnemyState::IDLE;
			return;
		}

		if (_state == EnemyState::SPAWNING && _enemy_body)
		{
			_player_camera->Shake(0.5f, 0.8f);
			_enemy_body->SetActive(true);
			_big_dust_particles->Restart();
			//Push the player back
			int hit = _combat_manager->EnemyMeleeAttack(transform->GetGlobalMatrix(), push_attack);

			if (hit > 0)
			{
				_combat_visual_effects_pool->PlayEnemyAttackEffect(_enemy_type, _player_pos);
			}

			_attack_cooldown = _combat_stats->_attack_cd;
			return;
		}

		_current_spawning_time -= Time::DeltaTimeScaled();
		if (_current_spawning_time <= 0.0f)
		{
			_state = EnemyState::SPAWNING;
			animation->ResetState();
		}
		break;
	}
}

void Hachiko::Scripting::EnemyController::DeathController()
{
	float transition;
	switch (_state)
	{
	case EnemyState::PARASITE:

		_enemy_body->SetActive(false);
		_parasite_dissolving_time_progress += Time::DeltaTimeScaled();
		transition = math::Sqrt(_parasite_dissolve_time - _parasite_dissolving_time_progress) * _parasite_dissolving;
		_parasite->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, transition), true);

		if (_parasite_dissolving_time_progress >= _parasite_dissolve_time)
		{
			DestroyEntity();
		}
		break;
	case EnemyState::HIDEN:
		if (animation->IsAnimationStopped())
		{
			DestroyEntity();
		}
		break;
	case EnemyState::DEAD:
		if (animation->IsAnimationStopped())
		{
			_enemy_dissolving_time_progress += Time::DeltaTimeScaled();
			if (_enemy_dissolve_time >= _enemy_dissolving_time_progress)
			{
				transition = math::Sqrt(_enemy_dissolve_time - _enemy_dissolving_time_progress) * _enemy_dissolving;
				_enemy_body->ChangeDissolveProgress(transition, true);
			}
			else
			{
				_enemy_body->SetActive(false);
				DropParasite();
			}
		}
		break;
	default:
		if (!_will_die && _enemy_type == EnemyType::WORM)
		{
			_audio_manager->UnregisterCombat();
			_state = EnemyState::HIDEN;
			game_object->GetComponent<ComponentAgent>()->RemoveFromCrowd();
			return;
		}
		else
		{
			if (_enemy_type == EnemyType::WORM)
			{
				_audio_manager->UnregisterCombat();
			}
			_state = EnemyState::DEAD;
			game_object->GetComponent<ComponentAgent>()->RemoveFromCrowd();
			return;
		}
	}
}

void Hachiko::Scripting::EnemyController::GetComponents()
{
	_player = Scenes::GetPlayer();
	_player_controller = _player->GetComponent<PlayerController>();

	animation = game_object->GetComponent<ComponentAnimation>();
	transform = game_object->GetTransform();

	_combat_manager = Scenes::GetCombatManager()->GetComponent<CombatManager>();
	_player_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
	_combat_visual_effects_pool = Scenes::GetCombatVisualEffectsPool()->GetComponent<CombatVisualEffectsPool>();
	_audio_manager_game_object = Scenes::GetAudioManager();

	_audio_source = game_object->GetComponent<ComponentAudioSource>();
	_audio_manager = _audio_manager_game_object->GetComponent<AudioManager>();
}

void Hachiko::Scripting::EnemyController::SetStats()
{
	_combat_stats = game_object->GetComponent<Stats>();
	_combat_stats->_attack_power = 1;
	_combat_stats->_current_hp = _combat_stats->_max_hp;
	_stun_time = 0.0f;
	_is_stunned = false;
	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		_acceleration = game_object->GetComponent<ComponentAgent>()->GetMaxAcceleration();
		_speed = game_object->GetComponent<ComponentAgent>()->GetMaxSpeed();
		break;
	}
}

void Hachiko::Scripting::EnemyController::SetVfx()
{
	if (_blood_trail != nullptr)
	{
		_blood_trail_particles = _blood_trail->GetComponent<ComponentParticleSystem>();
	}

	switch (_enemy_type)
	{
	case EnemyType::WORM:
		SetUpWormVfx();
		break;
	}
}

void Hachiko::Scripting::EnemyController::SetUpWormVfx()
{
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
	if (_projectile_particles)
	{
		_projectile_particles_comp = _projectile_particles->GetComponent<ComponentParticleSystem>();
		if (_projectile_particles_comp)
		{
			_projectile_particles_comp->Stop();
		}
	}
	if (_explosion_particles)
	{
		_explosion_particles_comp = _explosion_particles->GetComponent<ComponentParticleSystem>();
		if (_explosion_particles_comp)
		{
			_explosion_particles_comp->Stop();
		}
	}
}

void Hachiko::Scripting::EnemyController::StunController()
{
	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		BeetleStunController();
		break;
	case EnemyType::WORM:
		WormStunController();
		break;
	}
}

void Hachiko::Scripting::EnemyController::BeetleStunController()
{
	if (_is_stunned)
	{
		if (_stun_time > 0.0f)
		{
			// Run the timer for stun in unscaled time:
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

void Hachiko::Scripting::EnemyController::WormStunController()
{
	if (_state == EnemyState::HIT)
	{
		//_immune = true;
		_is_stunned = true;
	}
	else
	{
		_immune = false;
		_is_stunned = false;
	}

	if (_previous_state == EnemyState::HIT)
	{
		if (animation->IsAnimationStopped())
		{
			_state = EnemyState::IDLE;
		}
	}
}

void Hachiko::Scripting::EnemyController::MovementController()
{
	switch (_enemy_type)
	{
	case EnemyType::WORM:
		WormMovementController();
		break;
	case EnemyType::BEETLE:
		BeetleMovementController();
		break;
	}
}

void Hachiko::Scripting::EnemyController::WormMovementController()
{
	_enemy_body->GetTransform()->LookAtTarget(_player_pos);
}

void Hachiko::Scripting::EnemyController::BeetleMovementController()
{
	if (_state == EnemyState::ATTACKING && animation->IsAnimationStopped())
	{
		_state = EnemyState::IDLE;
	}

	if (_state == EnemyState::IDLE)
	{
		_current_idle_cooldown -= Time::DeltaTimeScaled();
		if (_current_idle_cooldown <= 0.0f)
		{
			PatrolMovement();
		}
	}

	if (_state == EnemyState::PATROL)
	{
		if (_current_pos.Distance(_target_pos) > 0.3f)
		{
			_state = EnemyState::IDLE;
			_current_idle_cooldown = _idle_cooldown;
		}
	}
}

void Hachiko::Scripting::EnemyController::AttackController()
{
	_attack_cooldown -= Time::DeltaTimeScaled();
	_attack_cooldown = _attack_cooldown < 0.0f ? 0.0f : _attack_cooldown;

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		BeetleAttackController();
		break;
	case EnemyType::WORM:
		WormAttackController();
		break;
	}
}

void Hachiko::Scripting::EnemyController::BeetleAttackController()
{
	_enraged -= Time::DeltaTimeScaled();
	_enraged = _enraged < 0.0f ? 0.0f : _enraged;

	if (IsAttacking())
	{
		_attack_animation_timer += Time::DeltaTimeScaled();

		if (_attack_animation_timer >= _attack_animation_duration)
		{
			_attack_animation_timer = 0.0f;
			_state = EnemyState::IDLE;
			_current_idle_cooldown = _idle_cooldown;
		}
	}
	else 
	{
		_state = EnemyState::IDLE;
	}

	float dist_to_player = _current_pos.Distance(_player_pos);

	// If an enemy is from a gautlet, it will always follow the player
	if ((_is_from_gautlet || dist_to_player < _aggro_range || _enraged > 0.0f) && _player_controller->IsAlive())
	{
		if (dist_to_player <= _attack_range || IsAttacking())
		{
			BeetleAttack();
		}
		else
		{
			ChasePlayer();
		}
	}
}

void Hachiko::Scripting::EnemyController::BeetleAttack()
{
	if (_state == EnemyState::DEAD || _attack_cooldown > 0.0f)
	{
		return;
	}

	// If no current attack start attack
	if (_attack_current_delay <= 0.0f || _previous_state != EnemyState::ATTACKING)
	{
		_attack_current_delay = _attack_delay;
		_state = EnemyState::ATTACKING;
	}
	else
	{
		_current_idle_cooldown = _idle_cooldown;
	}

	// If attacking lower attack delay
	if (_previous_state == EnemyState::ATTACKING)
	{
		_attack_current_delay -= Time::DeltaTimeScaled();
	}

	// Dont process attack untill its finished
	if (_attack_current_delay > 0.0f)
	{
		return;
	}

	// Reset cooldown and process attack
	_attack_cooldown = _combat_stats->_attack_cd;

	CombatManager::AttackStats attack_stats;
	attack_stats.damage = _combat_stats->_attack_power;
	attack_stats.knockback_distance = 0.0f;
	attack_stats.width = 4.f;
	attack_stats.range = _attack_range * 1.1f; // a bit bigger than its attack activation range
	attack_stats.type = CombatManager::AttackType::RECTANGLE;

	Debug::DebugDraw(_combat_manager->CreateAttackHitbox(GetMeleeAttackOrigin(attack_stats.range), attack_stats), float3(1.0f, 1.0f, 0.0f));

	int hit = _combat_manager->EnemyMeleeAttack(GetMeleeAttackOrigin(attack_stats.range), attack_stats);

	if (hit > 0)
	{
		_combat_visual_effects_pool->PlayEnemyAttackEffect(_enemy_type, _player_pos);
	}

	_attack_alt = !_attack_alt;

	transform->LookAtTarget(_player_pos);
	StopMoving();
}

float4x4 Hachiko::Scripting::EnemyController::GetMeleeAttackOrigin(float attack_range) const
{
	ComponentTransform* enemy_transform = game_object->GetTransform();
	float3 emitter_direction = enemy_transform->GetFront().Normalized();
	float3 emitter_position = _current_pos + emitter_direction * (attack_range / 2.f);
	float4x4 emitter = float4x4::FromTRS(emitter_position, enemy_transform->GetGlobalRotation(), enemy_transform->GetGlobalScale());
	return emitter;
}

void Hachiko::Scripting::EnemyController::ChasePlayer()
{
	if (_state == EnemyState::DEAD)
	{
		return;
	}

	_state = EnemyState::MOVING;

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

void Hachiko::Scripting::EnemyController::PatrolMovement()
{
	_state = EnemyState::PATROL;

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

void Hachiko::Scripting::EnemyController::StopMoving()
{
	_target_pos = transform->GetGlobalPosition();
	MoveInNavmesh();
}

void Hachiko::Scripting::EnemyController::MoveInNavmesh()
{
	ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
	agc->SetTargetPosition(_target_pos);
}

void Hachiko::Scripting::EnemyController::WormAttackController()
{
	if (_attack_cooldown > 0.0f)
	{
		return;
	}

	float dist_to_player = _current_pos.Distance(_player_pos);

	if (_previous_state == EnemyState::IDLE && _state != EnemyState::ATTACKING && !_attack_landing && dist_to_player <= _attack_range && _player_controller->IsAlive())
	{
		_state = EnemyState::ATTACKING;
		_attack_current_delay = _attack_delay;
		return;
	}

	if (_state == EnemyState::ATTACKING && animation->IsAnimationStopped())
	{
		_state = EnemyState::IDLE;
	}

	_attack_current_delay -= Time::DeltaTimeScaled();
	if (_attack_current_delay <= 0.0f)
	{
		_attack_current_delay = 0;
	}

	if (!_attack_landing && _attack_current_delay <= 0.0f && dist_to_player <= _attack_range && _player_controller->IsAlive())
	{
		// We create the attack zone after the delay
		_attack_zone->GetTransform()->SetGlobalPosition(_player_pos);
		_attack_landing = true;
		_inner_indicator_billboard->Play();
		_outer_indicator_billboard->Play();
		_attack_animation_timer = 0.0f;
		_projectile_particles_comp->Play();
		return;
	}

	if (_attack_landing)
	{
		_attack_animation_timer += Time::DeltaTimeScaled();
		if (_attack_animation_timer >= 1.0f)
		{
			CombatManager::AttackStats attack_stats;
			attack_stats.damage = _combat_stats->_attack_power;
			attack_stats.knockback_distance = 0.0f;
			attack_stats.width = 0.f;
			attack_stats.range = 2.5; // a bit bigger than its attack activation range
			attack_stats.type = CombatManager::AttackType::CIRCLE;


			int hit = _combat_manager->EnemyMeleeAttack(_attack_zone->GetTransform()->GetGlobalMatrix(), attack_stats);

			if (hit > 0)
			{
				_combat_visual_effects_pool->PlayEnemyAttackEffect(_enemy_type, _player_pos);
			}

			_attack_landing = false;
			_attack_cooldown = _combat_stats->_attack_cd;

			_explosion_particles->GetTransform()->SetGlobalPosition(_attack_zone->GetTransform()->GetGlobalPosition());
			_explosion_particles_comp->Play();

			_projectile_particles_comp->Stop();
		}
		else if (_attack_animation_timer >= .5f)
		{
			float3 offset_attack_pos = _attack_zone->GetTransform()->GetGlobalPosition();
			offset_attack_pos.y += 50;
			_projectile_particles->GetTransform()->SetGlobalPosition(math::float3::Lerp(offset_attack_pos, _attack_zone->GetTransform()->GetGlobalPosition(), _attack_animation_timer));
		}
		else
		{
			float3 offset_worm_pos = transform->GetGlobalPosition();
			offset_worm_pos.y += 50;
			_projectile_particles->GetTransform()->SetGlobalPosition(math::float3::Lerp(transform->GetGlobalPosition(), offset_worm_pos, _attack_animation_timer));
		}
	}
}

void Hachiko::Scripting::EnemyController::DestroyEntity()
{
	if (_is_from_boss)
	{
		ResetEnemy();
		ResetEnemyPosition();

		ComponentAgent* agent = game_object->GetComponent<ComponentAgent>();
		if (agent)
		{
			agent->RemoveFromCrowd();
		}
	}

	game_object->SetActive(false);
}

void Hachiko::Scripting::EnemyController::DropParasite()
{
	StopMoving();
	_state = EnemyState::PARASITE;
	//TODO: Check if in scene there's already a parasite? Maybe?
	if (_parasite) 
	{
		_parasite->SetActive(true);
	}

	_parasite_dropped = true;
}

void Hachiko::Scripting::EnemyController::RegisterHit(int damage, float3 direction, float knockback, bool is_from_player, bool is_ranged)
{
	if (_state == EnemyState::SPAWNING || !_enemy_body->IsActive() || _immune)
	{
		return; // Immune while spawning or if it isn't there
	}

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		_knockback_pos = transform->GetGlobalPosition() + (direction * knockback);
		break;
	case EnemyType::WORM:
		_knockback_pos = transform->GetGlobalPosition();
		break;
	}

	_state = EnemyState::HIT;

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

void Hachiko::Scripting::EnemyController::GetParasite()
{
	DestroyEntity();
}

void Hachiko::Scripting::EnemyController::Spawn()
{
	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
		if (_enemy_body)
		{
			_enemy_body->SetActive(true);
		}
		if (_parasite)
		{
			_parasite->SetActive(false);
		}
		_current_spawning_time = _spawning_time;
		_has_spawned = true;
		_state = EnemyState::SPAWNING;
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
		_state = EnemyState::INVALID;
		_small_dust_particles->Restart();
		_current_spawning_time = _spawning_time;
		_player_camera->Shake(_spawning_time, 0.8f);
		break;
	}
}

void Hachiko::Scripting::EnemyController::CheckState()
{
	EnemyState current_state = GetState();
	bool state_changed = current_state != _previous_state;

	if (!state_changed)
	{
		return;
	}

	if (((_previous_state == EnemyState::ATTACKING || _previous_state == EnemyState::MOVING) &&
		(current_state == EnemyState::IDLE || current_state == EnemyState::MOVING_BACK || current_state == EnemyState::DEAD))
		|| (_enemy_type == EnemyType::WORM && (current_state == EnemyState::DEAD || (current_state == EnemyState::HIDEN))))
	{
		if (_already_in_combat)
		{
			_audio_manager->UnregisterCombat();
			_already_in_combat = false;
		}
	}
	else if (((current_state == EnemyState::ATTACKING || current_state == EnemyState::MOVING) &&
		(_previous_state == EnemyState::IDLE || _previous_state == EnemyState::MOVING_BACK || _previous_state == EnemyState::DEAD))
		|| (_enemy_type == EnemyType::WORM && current_state != EnemyState::DEAD && current_state != EnemyState::HIDEN))
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
	case EnemyState::IDLE:
		animation->SendTrigger("idle");
		break;
	case EnemyState::ATTACKING:
		_audio_source->PostEvent(Sounds::ENEMY_ATTACK);
		if (!_attack_alt)
		{
			animation->SendTrigger("isAttacking");
		}
		else
		{
			animation->SendTrigger("isAttackingAlt");
		}
		break;
	case EnemyState::DEAD:
		_audio_source->PostEvent(Sounds::ENEMY_DIE);
		animation->SendTrigger("isDead");
		break;
	case EnemyState::MOVING:
		animation->SendTrigger("isMoving");
		break;
	case EnemyState::MOVING_BACK:
		animation->SendTrigger("isMoving");
		break;
	case EnemyState::PATROL:
		animation->SendTrigger("idle");
		break;
	case EnemyState::HIT:
		animation->SendTrigger("isHit");
		break;
	case EnemyState::HIDEN:
		animation->SendTrigger("isHide");
		break;
	case EnemyState::SPAWNING:
		animation->SendTrigger("isAppear");
		break;
	case EnemyState::INVALID:
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
	_state = EnemyState::INVALID;
	_previous_state = EnemyState::INVALID;
	_parasite_dissolving_time_progress = 0.f;
	_enemy_dissolving_time_progress = 0.f;
	_parasite_dropped = false;

	if (_enemy_body)
	{
		_enemy_body->SetActive(true);
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

	if (_explosion_particles_comp != nullptr)
	{
		_explosion_particles_comp->Stop();
	}

	if (_projectile_particles_comp != nullptr)
	{
		_projectile_particles_comp->Stop();
	}
}

void Hachiko::Scripting::EnemyController::ResetEnemyPosition()
{
	transform->SetGlobalPosition(_spawn_pos);
	transform->SetGlobalRotation(_spawn_rot);
}
