#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

#define RAD_TO_DEG 180.0f / math::pi
const int MAX_AMMO = 4;

Hachiko::Scripting::PlayerController::PlayerController(GameObject* game_object)
	: Script(game_object, "PlayerController")
	, _attack_indicator(nullptr)
	, _bullet_emitter(nullptr)
	, _goal(nullptr)
	, _geo(nullptr)
	, _dash_duration(0.0f)
	, _dash_distance(0.0f)
	, _dash_cooldown(0.0f)
	, _invulnerability_time(1.0f)
	, _dash_scaler(3)
	, _rotation_duration(0.0f)
	, _combat_stats()
	, _max_dash_charges(0)
	, _state(PlayerState::INVALID)
	, _camera(nullptr)
	, _ui_damage(nullptr)
	, _dash_trail(nullptr)
	, _sword_weapon(nullptr)
	, _sword_upper(nullptr)
	, _claw_weapon(nullptr)
	, _trail_enlarger(10.0f)
{
	CombatManager::BulletStats common_bullet;
	common_bullet.charge_time = .5f;
	common_bullet.lifetime = 3.f;
	common_bullet.size = 1.f;
	common_bullet.speed = 50.f;
	common_bullet.damage = 1.f;
	
	Weapon melee;
	melee.name = "Melee";
	melee.bullet = common_bullet;
	melee.color = float4(255.0f, 0.0f, 0.0f, 255.0f);
	melee.attacks.push_back(GetAttackType(AttackType::COMMON_1));
	melee.attacks.push_back(GetAttackType(AttackType::COMMON_2));
	melee.attacks.push_back(GetAttackType(AttackType::COMMON_3));

	Weapon claw;
	claw.name = "Claw";
	claw.bullet = common_bullet;
	claw.color = float4(0.0f, 0.0f, 255.0f, 255.0f);
	claw.attacks.push_back(GetAttackType(AttackType::QUICK_1));
	claw.attacks.push_back(GetAttackType(AttackType::QUICK_2));
	claw.attacks.push_back(GetAttackType(AttackType::QUICK_3));
	
	Weapon sword;
	sword.name = "Sword";
	sword.bullet = common_bullet;
	sword.color = float4(0.0f, 255.0f, 0.0f, 255.0f);;
	sword.attacks.push_back(GetAttackType(AttackType::HEAVY_1));
	sword.attacks.push_back(GetAttackType(AttackType::HEAVY_2));
	sword.attacks.push_back(GetAttackType(AttackType::HEAVY_3));

	weapons.push_back(melee);
	weapons.push_back(claw);
	weapons.push_back(sword);

	_current_weapon = 0;
	_current_cam_setting = 0;
}

void Hachiko::Scripting::PlayerController::OnAwake()
{
	_terrain = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Level");
	_enemies = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");
	_level_manager = game_object->scene_owner->GetRoot()->GetFirstChildWithName("LevelManager")->GetComponent<LevelManager>();

	_dash_charges = _max_dash_charges;

	if (_attack_indicator)
	{
		_attack_indicator->SetActive(false);
	}
	if (_ui_damage)
	{
		_ui_damage->SetActive(false);
	}

	if (_dash_trail)
	{
		_dash_trail->SetActive(false);
	}

	_falling_dust_particles = _falling_dust->GetComponent<ComponentParticleSystem>();
	_walking_dust_particles = _walking_dust->GetComponent<ComponentParticleSystem>();

	_combat_stats = game_object->GetComponent<Stats>();
	// Player doesnt use all combat stats since some depend on weapon
	_combat_stats->_attack_power = 2;
	_combat_stats->_attack_cd = 1;
	_combat_stats->_move_speed = 7.0f;
	_combat_stats->_max_hp = 4;
	_combat_stats->_current_hp = _combat_stats->_max_hp;


	if (!_ammo_cell_1 || !_ammo_cell_2 || !_ammo_cell_3 || !_ammo_cell_4)
	{
		HE_LOG("Error loading Ammo Cells UI");
		return;
	}

	ammo_cells.push_back(_ammo_cell_1);
	ammo_cells.push_back(_ammo_cell_2);
	ammo_cells.push_back(_ammo_cell_3);
	ammo_cells.push_back(_ammo_cell_4);
	_ammo_count = MAX_AMMO;

	// First position and rotation set if no camera is found
	_cam_positions.push_back(float3(0.0f, 19.0f, 13.0f));
	_cam_rotations.push_back(float3(125.0f, 0.0f, 180.0f));

	_cam_positions.push_back(float3(0.0f, 4.0f, 12.0f));
	_cam_rotations.push_back(float3(165.0f, 0.0f, 180.0f));

	if (_camera != nullptr)
	{
		_cam_positions[0] = _camera->GetComponent<PlayerCamera>()->GetRelativePosition();
		_cam_rotations[0] = _camera->GetTransform()->GetLocalRotationEuler();
	}
	if (!_hp_cell_1 || !_hp_cell_2 || !_hp_cell_3 || !_hp_cell_4)
	{
		HE_LOG("Error loading HP Cells UI");
		return;
	}

	hp_cells.push_back(_hp_cell_1);
	hp_cells.push_back(_hp_cell_2);
	hp_cells.push_back(_hp_cell_3);
	hp_cells.push_back(_hp_cell_4);
}

void Hachiko::Scripting::PlayerController::OnStart()
{
	animation = game_object->GetComponent<ComponentAnimation>();
	animation->StartAnimating();
	_initial_pos = game_object->GetTransform()->GetGlobalPosition();

	_level_manager->SetRespawnPosition(game_object->GetTransform()->GetGlobalPosition());

	if (_dash_trail) // Init dash trail start/end positions and scale
	{
		/*The relation for start_pos and start_scale must be always --> start_scale.x = -start_pos.z / 2
		* make sure to set correctly the local transform in the engine
		* E.g. if scale = (5, Y, Z) then position = (X, Y, -2.5)
		*/
		_trail_start_pos = _dash_trail->GetTransform()->GetLocalPosition();
		_trail_start_scale = _dash_trail->GetTransform()->GetLocalScale();

		//Position only applies on -Z axis
		_trail_end_pos = _dash_trail->GetTransform()->GetLocalPosition();
		_trail_end_pos = math::float3(_trail_end_pos.x, _trail_end_pos.y, _trail_end_pos.z * _trail_enlarger);

		//Scale only applies on +X axis
		_trail_end_scale = _dash_trail->GetTransform()->GetLocalScale();
		_trail_end_scale = math::float3(_trail_end_scale.x * _trail_enlarger, _trail_end_scale.y, _trail_end_scale.z);
	}
}

void Hachiko::Scripting::PlayerController::OnUpdate()
{

	_player_transform = game_object->GetTransform();
	_player_position = _player_transform->GetGlobalPosition();
	_movement_direction = float3::zero;

	if (!IsAlive())
	{
		_state = PlayerState::DIE;
	}

	if (animation->IsAnimationStopped() && IsDying())
	{
		_state = PlayerState::IDLE;
		_level_manager->Respawn(this);
	}

	if (_invulnerability_time_remaining > 0.0f)
	{
		_invulnerability_time_remaining -= Time::DeltaTime();
	}

	if (_god_mode_trigger)
	{
		_god_mode = !_god_mode;
		ToggleGodMode();
		_god_mode_trigger = false;
	}
	// Handle player the input
	HandleInputAndStatus();

	// Run attack simulation
	AttackController();

	// Run movement simulation
	MovementController();

	// Rotate player to the necessary direction:
	WalkingOrientationController();

	// Apply the position after everything is simulated
	_player_transform->SetGlobalPosition(_player_position);

	CheckState();
}

Hachiko::Scripting::PlayerState Hachiko::Scripting::PlayerController::GetState() const
{
	return _state;
}

math::float3 Hachiko::Scripting::PlayerController::GetRaycastPosition(
	const math::float3& current_position) const
{
	const math::Plane plane(math::float3(0.0f, current_position.y, 0.0f),
		math::float3(0.0f, 1.0f, 0.0f));

	const math::float2 mouse_position_view =
		ComponentCamera::ScreenPositionToView(Input::GetMouseNormalizedPosition());

	const math::LineSegment ray = Debug::GetRenderingCamera()->Raycast(
		mouse_position_view.x, mouse_position_view.y);

	return plane.ClosestPoint(ray);
}

float3 Hachiko::Scripting::PlayerController::GetCorrectedPosition(const float3& target_pos) const
{
	return Navigation::GetCorrectedPosition(target_pos, float3(0.5f, 0.5f, 0.5f));
}

void Hachiko::Scripting::PlayerController::SpawnGameObject() const
{
	static int times_hit_g = 0;

	if (!Input::IsKeyDown(Input::KeyCode::KEY_G))
	{
		return;
	}

	std::string name = "GameObject ";

	name += std::to_string(times_hit_g);

	times_hit_g++;

	GameObject* created_game_object = GameObject::Instantiate();

	created_game_object->SetName(name);
}

void Hachiko::Scripting::PlayerController::HandleInputAndStatus()
{
	// Movement Direction
	if (Input::IsKeyPressed(Input::KeyCode::KEY_W))
	{
		_movement_direction -= math::float3::unitZ;
	}
	else if (Input::IsKeyPressed(Input::KeyCode::KEY_S))
	{
		_movement_direction += math::float3::unitZ;
	}

	if (Input::IsKeyPressed(Input::KeyCode::KEY_D))
	{
		_movement_direction += math::float3::unitX;
	}
	else if (Input::IsKeyPressed(Input::KeyCode::KEY_A))
	{
		_movement_direction -= math::float3::unitX;
	}

	if (!IsActionLocked())
	{
		if (!IsAttackOnCooldown() && _ammo_count > 0 && Input::IsMouseButtonDown(Input::MouseButton::RIGHT))
		{
			RangedAttack();
		}
		else if (!IsAttackOnCooldown() && (Input::IsMouseButtonDown(Input::MouseButton::LEFT) || GetBufferedClick() == Input::MouseButton::LEFT))
		{
			MeleeAttack();
		}
		// Keep dash here since it uses the input movement direction
		else if (Input::IsKeyDown(Input::KeyCode::KEY_SPACE) && _dash_charges > 0)
		{
			Dash();
		}
		else if (!_movement_direction.Equals(float3::zero))
		{
			_state = PlayerState::WALKING;
		}
		else
		{
			_state = PlayerState::IDLE;
		}

		if (Input::IsKeyDown(Input::KeyCode::KEY_F))
		{
			PickupParasite(_player_position);
		}
	}	
	else
	{
		HandleInputBuffering();
	}

	// Range attack charge cancel
	if (_state == PlayerState::RANGED_ATTACKING && Input::IsMouseButtonUp(Input::MouseButton::RIGHT))
	{
		CancelAttack();
	}

	if (_god_mode && Input::IsKeyPressed(Input::KeyCode::KEY_Q))
	{
		_player_position += math::float3::unitY * 0.5f;
	}
	else if (_god_mode && Input::IsKeyPressed(Input::KeyCode::KEY_E))
	{
		_player_position -= math::float3::unitY * 0.5f;
	}

	if (Input::IsKeyDown(Input::KeyCode::KEY_G))
	{
		_god_mode = !_god_mode;

		ToggleGodMode();
	}
	
	// Testing for camera
	if (Input::IsKeyDown(Input::KeyCode::KEY_C))
	{
		++_current_cam_setting;
		if (_current_cam_setting >= _cam_positions.size())
		{
			_current_cam_setting = 0;
		}
		_camera->GetComponent<PlayerCamera>()->SwitchRelativePosition(_cam_positions[_current_cam_setting], 1.0f);
		_camera->GetComponent<PlayerCamera>()->RotateCameraTo(_cam_rotations[_current_cam_setting], 1.0f);
	}
}

void Hachiko::Scripting::PlayerController::HandleInputBuffering()
{
	if (Input::IsMouseButtonDown(Input::MouseButton::LEFT))
	{
		// Melee combo input buffer
		click_buffer.push(Input::MouseButton::LEFT);
	}
}

Hachiko::Input::MouseButton Hachiko::Scripting::PlayerController::GetBufferedClick()
{
	Input::MouseButton next_click = Input::MouseButton::UNKNOWN;

	if (click_buffer.empty())
	{
		return next_click;
	}

	if (_attack_idx == GetCurrentWeapon().attacks.size() - 1)
	{
		ResetClickBuffer();
		return next_click;
	}

	next_click = click_buffer.front();
	click_buffer.pop();
	return next_click;
}

void Hachiko::Scripting::PlayerController::ResetClickBuffer()
{
	std::queue<Input::MouseButton> new_buffer;
	std::swap(click_buffer, new_buffer);
}

void Hachiko::Scripting::PlayerController::Dash()
{
	_state = PlayerState::DASHING;
	_dash_charges -= 1;
	_dash_progress = 0.f;
	_dash_start = _player_position;
	_current_dash_duration = _dash_duration;

	// If we are not inputing any direction default to player orientation
	if (_movement_direction.Equals(float3::zero))
	{
		_dash_direction = _player_transform->GetFront();
	}
	else
	{
		_dash_direction = _movement_direction;
	}
	_dash_direction.Normalize();

	float3 corrected_dash_final_position;
	_dash_end = _dash_start + _dash_direction * _dash_distance;
	// Correct by wall hit
	CorrectDashDestination(_dash_start, _dash_end);
}

void Hachiko::Scripting::PlayerController::CorrectDashDestination(const float3& dash_source, float3& dash_destination)
{
	float3 corrected_dash_destination;
	bool hit_terrain = GetTerrainCollision(dash_source, dash_destination, corrected_dash_destination);
	if (hit_terrain)
	{
		dash_destination = corrected_dash_destination;
		// Get corrected position with a lot of width radius (navmesh seems to not always match the wall properly)
		corrected_dash_destination = Navigation::GetCorrectedPosition(dash_destination, float3(5.f, 0.5f, 5.f));
	}
	else
	{
		// Correct normally by navmesh
		corrected_dash_destination = GetCorrectedPosition(dash_destination);
	}
	if (corrected_dash_destination.x < FLT_MAX)
	{
		dash_destination = corrected_dash_destination;
	}
}

void Hachiko::Scripting::PlayerController::MeleeAttack()
{
	_state = PlayerState::MELEE_ATTACKING;
	const Weapon& weapon = GetCurrentWeapon();
	const PlayerAttack& attack = GetNextAttack();
	_attack_current_duration = attack.duration;
	_after_attack_timer = attack.cooldown +_combo_grace_period;
	
	// Attack will occur in the attack simulation after the delay
	_attack_current_delay = attack.hit_delay;

	_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
	CombatManager* combat_manager = _bullet_emitter->GetComponent<CombatManager>();

	// Move player a bit forward if it wouldnt fall	
	_dash_progress = 0.f;
	_current_dash_duration = attack.duration;
	_dash_start = _player_position;
	_dash_end = _player_position + _player_transform->GetFront().Normalized() * attack.dash_distance;
	CorrectDashDestination(_dash_start, _dash_end);

	// Fast and Scuffed, has to be changed when changing attack indicator
	float4 attack_color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	_attack_indicator->ChangeEmissiveColor(attack_color, attack.duration, true);
}

void Hachiko::Scripting::PlayerController::ChangeGOWeapon()
{
	if (_current_weapon == 0) { // MELEE
		_claw_weapon->SetActive(false);
		_sword_upper->SetActive(false);
		_sword_weapon->SetActive(false);
	}
	else if (_current_weapon == 1) // CLAW
	{
		_claw_weapon->SetActive(true);
		_sword_upper->SetActive(false);
		_sword_weapon->SetActive(false);
	}
	else if (_current_weapon == 2) // SWORD
	{
		_claw_weapon->SetActive(false);
		_sword_upper->SetActive(true);
		_sword_weapon->SetActive(true);
	}
}

bool Hachiko::Scripting::PlayerController::IsAttacking() const
{
	return _state == PlayerState::MELEE_ATTACKING || _state == PlayerState::RANGED_ATTACKING;
}

bool Hachiko::Scripting::PlayerController::IsDashing() const
{
	return _state == PlayerState::DASHING;
}

bool Hachiko::Scripting::PlayerController::IsWalking() const
{
	return _state == PlayerState::WALKING;
}

bool Hachiko::Scripting::PlayerController::IsStunned() const
{
	return _state == PlayerState::STUNNED;
}

bool Hachiko::Scripting::PlayerController::IsFalling() const
{
	return _state == PlayerState::FALLING;
}

bool Hachiko::Scripting::PlayerController::IsPickUp() const
{
	return _state == PlayerState::PICK_UP;
}

bool Hachiko::Scripting::PlayerController::IsDying() const
{
	return _state == PlayerState::DIE;
}

bool Hachiko::Scripting::PlayerController::IsActionLocked() const
{
	return IsDashing() || IsStunned() || IsAttacking() || IsFalling() || IsPickUp() || IsDying();
}

bool Hachiko::Scripting::PlayerController::IsAttackOnCooldown() const
{
	return _after_attack_timer - _combo_grace_period > 0;
}

bool Hachiko::Scripting::PlayerController::IsInComboWindow() const
{
	return _after_attack_timer > 0;
}

const Hachiko::Scripting::PlayerController::Weapon& Hachiko::Scripting::PlayerController::GetCurrentWeapon() const
{
	return weapons[_current_weapon];
}

const Hachiko::Scripting::PlayerController::PlayerAttack& Hachiko::Scripting::PlayerController::GetNextAttack()
{
	if (IsInComboWindow())
	{
		++_attack_idx;
		if (_attack_idx >= GetCurrentWeapon().attacks.size())
		{
			_attack_idx = 0;
		}
	}
	else
	{
		_attack_idx = 0;
	}
	return GetCurrentAttack();
}

const Hachiko::Scripting::PlayerController::PlayerAttack& Hachiko::Scripting::PlayerController::GetCurrentAttack() const
{
	return GetCurrentWeapon().attacks[_attack_idx];
}

void Hachiko::Scripting::PlayerController::RangedAttack()
{
	
	_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
	const float3 forward = _player_transform->GetFront().Normalized();

	CombatManager* bullet_controller = _bullet_emitter->GetComponent<CombatManager>();
	if (bullet_controller)
	{
		CombatManager::BulletStats stats = GetCurrentWeapon().bullet;
		_attack_current_duration = stats.charge_time;
		_current_bullet = bullet_controller->ShootBullet(_player_transform, stats);
		if (_current_bullet >= 0)
		{
			math::Clamp(--_ammo_count, 0, MAX_AMMO);
			_state = PlayerState::RANGED_ATTACKING;
		}
	}
	_after_attack_timer = _ranged_attack_cooldown;
}

void Hachiko::Scripting::PlayerController::CancelAttack()
{
	// Indirectly cancells atack by putting its remaining duration to 0
	_attack_current_duration = 0.f;
	if (_state == PlayerState::RANGED_ATTACKING && _current_bullet >= 0)
	{
		CombatManager* bullet_controller = _bullet_emitter->GetComponent<CombatManager>();
		bullet_controller->StopBullet(_current_bullet);
		math::Clamp(++_ammo_count, 0, MAX_AMMO);
	}
}

float4x4 Hachiko::Scripting::PlayerController::GetMeleeAttackOrigin(float attack_range) const
{
	float3 emitter_direction = _player_transform->GetFront().Normalized();
	float3 emitter_position = _player_transform->GetGlobalPosition() + emitter_direction * (attack_range / 2.f);
	float4x4 emitter = float4x4::FromTRS(emitter_position, _player_transform->GetGlobalRotation(), _player_transform->GetGlobalScale());
	return emitter;
}

bool Hachiko::Scripting::PlayerController::GetTerrainCollision(const float3& start, const float3& end, float3& collision_point) const
{
	constexpr bool active_only = true;
	GameObject* terrain_hit = SceneManagement::Raycast(start, end, &collision_point, _terrain, true);
	return terrain_hit != nullptr;
}

void Hachiko::Scripting::PlayerController::MovementController()
{
	DashController();
	WalkingOrientationController();

	if (IsWalking())
	{
		_player_position += (_movement_direction * _combat_stats->_move_speed * Time::DeltaTime());
		_walking_dust_particles->Play();
	}
	else
	{
		_walking_dust_particles->Stop();
	}

	if (_god_mode)
	{
		return;
	}

	if (IsPickUp() && animation->IsAnimationStopped())
	{
		_state = PlayerState::IDLE;
	}

	if (IsFalling())
	{
		constexpr float fall_speed = 25.f;
		_player_position.y -= fall_speed * Time::DeltaTime();

		if (_start_fall_pos.y - _player_position.y > _falling_distance)
		{
			_player_position = Navigation::GetCorrectedPosition(_dash_start, float3(5.0f, 5.0f, 5.0f));
		}
	}
	else if (IsStunned())
	{
		if(_stun_time > 0.0f)
		{
			_stun_time -= Time::DeltaTime();
			float stun_completion = (_stun_duration - _stun_time) * (1.0 / _stun_duration);
			_player_position = math::float3::Lerp(_knock_start, _knock_end,
				stun_completion);
			
		}
		else
		{
			_player_position = GetCorrectedPosition(_knock_end);
			_state = PlayerState::IDLE;
		}
	}

	float3 corrected_position = GetCorrectedPosition(_player_position);
	if (Distance(corrected_position, _player_position) < 1.0f)
	{
		_player_position = corrected_position;
		if (IsFalling())
		{
			// Stopped falling
			_state = PlayerState::IDLE;
			_falling_dust_particles->Restart();
		}
	}
	else if (!IsDashing())
	{
		// Started falling
		if (!IsFalling())
		{
			_start_fall_pos = _player_position;
			_state = PlayerState::FALLING;
		}
	}
}

void Hachiko::Scripting::PlayerController::DashController()
{
	DashChargesManager();

	if (!IsDashing() && _state != PlayerState::MELEE_ATTACKING)
	{
		return;
	}


	_dash_progress += Time::DeltaTime() / _dash_duration;
	_dash_progress = _dash_progress > 1.0f ? 1.0f : _dash_progress;
	
	// using y = x^p
	float acceleration = 1.0f - math::Pow((1.0f - _dash_progress) / 1.0f, (int)_dash_scaler);
	

	
	
	_player_position = math::float3::Lerp(_dash_start, _dash_end,
		acceleration);

	if (_state != PlayerState::MELEE_ATTACKING)
	{
		DashTrailManager(_dash_progress);
	}
	
	// Attack status is stopped in attack controller
	if (_dash_progress >= 1.0f && IsDashing())
	{
		_state = PlayerState::IDLE;
	}
}


void Hachiko::Scripting::PlayerController::DashChargesManager()
{
	if (_dash_charges == _max_dash_charges)
	{
		_dash_charging_time = 0.f;
		return;
	}

	_dash_charging_time += Time::DeltaTime();

	if (_dash_charging_time >= _dash_cooldown)
	{
		_dash_charging_time = 0.f;
		if (_dash_charges < _max_dash_charges)
		{
			_dash_charges += 1;
		}
	}
}

void Hachiko::Scripting::PlayerController::DashTrailManager(float dash_progress)
{
	if (!_show_dashtrail)
	{
		_show_dashtrail = true;
		_dash_trail->SetActive(_show_dashtrail);
	}
	
	_dash_trail->GetTransform()->SetLocalPosition(math::float3::Lerp(_trail_start_pos, _trail_end_pos,
		_dash_progress));
	_dash_trail->GetTransform()->SetLocalScale(math::float3::Lerp(_trail_start_scale, _trail_end_scale,
		_dash_progress));

	if (_dash_progress >= 1.0f)
	{
		_dash_trail->GetTransform()->SetLocalPosition(_trail_start_pos);
		_dash_trail->GetTransform()->SetLocalScale(_trail_start_scale);
		_show_dashtrail = false;
		_dash_trail->SetActive(_show_dashtrail);

	}
}

void Hachiko::Scripting::PlayerController::WalkingOrientationController()
{
	// If input tells to turn to player
	if (_state == PlayerState::WALKING)
	{
		// Get the rotation player is going to have:
		const math::Quat target_rotation = 
			_player_transform->SimulateLookAt(_movement_direction);

		// If rotation is gonna be changed fire up the rotating process:
		if (!target_rotation.Equals(_player_transform->GetGlobalRotation()))
		{
			// Reset all the variables related to rotation lerp:
			_rotation_progress = 0.0f;
			_rotation_target = target_rotation;
			_rotation_start = _player_transform->GetGlobalRotation();
			_should_rotate = true;
		}
	}

	// Smoothly rotate to the new direction of the player in _rotation_duration
	// amount of seconds:
	if (_should_rotate)
	{
		_rotation_progress += Time::DeltaTime() / _rotation_duration;
		_rotation_progress = _rotation_progress > 1.0f
			? 1.0f
			: _rotation_progress;

		_player_transform->SetGlobalRotation(Quat::Lerp(_rotation_start, _rotation_target, _rotation_progress));

		_should_rotate = _rotation_progress < 1.0f;

		if (!_should_rotate)
		{
			_rotation_progress = 0.0f;
			_rotation_target = math::Quat::identity;
			_rotation_start = math::Quat::identity;
			_should_rotate = false;
		}
	}
}

void Hachiko::Scripting::PlayerController::AttackController()
{
	if (!IsAttacking())
	{
		if (_after_attack_timer > 0.0f)
		{
			_after_attack_timer -= Time::DeltaTime();
		}
		return;
	}

	if (_attack_current_duration > 0.0f)
	{
		_attack_current_duration -= Time::DeltaTime();

		if (_state == PlayerState::MELEE_ATTACKING)
		{
			const PlayerAttack& attack = GetCurrentAttack();
			const Weapon& weapon = GetCurrentWeapon();
			CombatManager* combat_manager = _bullet_emitter->GetComponent<CombatManager>();

			if (attack.stats.type == CombatManager::AttackType::CONE)
			{
				_attack_indicator->SetActive(true);
			}
			else
			{
				if (combat_manager)
				{
					Debug::DebugDraw(combat_manager->CreateAttackHitbox(GetMeleeAttackOrigin(attack.stats.range), attack.stats), weapon.color.Float3Part());
				}
			}

			if (_attack_current_delay > 0.f)
			{
				_attack_current_delay -= Time::DeltaTime();
				
				if (_attack_current_delay <= 0.f)
				{
					int hit_count = 0;
					
					if (combat_manager)
					{
						// Offset the center of the attack if its a rectangle
						if (attack.stats.type == CombatManager::AttackType::RECTANGLE)
						{
							hit_count = combat_manager->PlayerMeleeAttack(GetMeleeAttackOrigin(attack.stats.range) , attack.stats);
						}
						else
						{
							hit_count = combat_manager->PlayerMeleeAttack(_player_transform->GetGlobalMatrix(), attack.stats);
						}
					}
					if (hit_count > 0)
					{
						_ammo_count = math::Clamp(_ammo_count += hit_count, 0, MAX_AMMO);
						UpdateAmmoUI();
						_camera->GetComponent<PlayerCamera>()->Shake(0.6f, 0.2f);
					}
				}
			}	
		}
		else
		{
			// Range attack
			_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
		}
	}

	if(_attack_current_duration <= 0)
	{
		if (_state == PlayerState::RANGED_ATTACKING)
		{
			// We only need the bullet reference to cancell it while charging
			_current_bullet = -1;
		}
		// Melee attack
		_attack_indicator->SetActive(false);
		
		// When attack is over
		_state = PlayerState::IDLE;
	}
}

void Hachiko::Scripting::PlayerController::PickupParasite(const float3& current_position)
{
	_state = PlayerState::PICK_UP;
	if (_enemies == nullptr) {
		return;
	}

	std::vector<GameObject*>& enemy_packs = _enemies->children;
	for (int i = 0; i < enemy_packs.size(); ++i)
	{
		GameObject* pack = enemy_packs[i];
		if (!pack->IsActive())
		{
			continue;
		}

		std::vector<GameObject*>& enemies = pack->children;


		for (int i = 0; i < enemies.size(); ++i)
		{
			if (enemies[i]->active && 1.5f >= _player_transform->GetGlobalPosition().Distance(enemies[i]->GetTransform()->GetGlobalPosition()))
			{
				EnemyController* enemy_controller = enemies[i]->GetComponent<EnemyController>();

				if (enemy_controller->IsAlive() == false && enemy_controller->ParasiteDropped())
				{
					enemy_controller->GetParasite();
					if (_geo != nullptr) 
					{
						_geo->ChangeEmissiveColor(float4(0.0f, 255.0f, 0.0f, 255.0f), 0.3f, true);
					}
					_combat_stats->Heal(1);
					UpdateHealthBar();
					// Generate a random number for the weapon
					std::random_device rd;
					std::mt19937 gen(rd());
					int num_of_weapons = static_cast <int>(WeaponUsed::SIZE) - 1;
					std::uniform_int_distribution<> dist(0, weapons.size() - 1);
					int new_wpn_num = dist(gen);
					_current_weapon = new_wpn_num;
					break;
				}
			}
		}

	}
	
	ChangeGOWeapon();
}

void Hachiko::Scripting::PlayerController::RegisterHit(float damage_received, bool is_heavy, float3 direction)
{
	if (_god_mode || _invulnerability_time_remaining > 0.0f)
	{
	    return;
	}

	_invulnerability_time_remaining = _invulnerability_time;
	_combat_stats->ReceiveDamage(damage_received);
	UpdateHealthBar();
	game_object->ChangeEmissiveColor(float4(255, 255, 255, 255), 0.3f, true);

	// Activate vignette
	if (_ui_damage && _combat_stats->_current_hp / _combat_stats->_max_hp < 0.25f)
	{
		_ui_damage->SetActive(true);
	}
	
	if(is_heavy)
	{
		RecieveKnockback(direction);
		_camera->GetComponent<PlayerCamera>()->Shake(0.5f, 0.5f);
	}
	else
	{
		if (_combat_stats->_current_hp / _combat_stats->_max_hp < 0.25f)
		{
			_camera->GetComponent<PlayerCamera>()->Shake(0.8f, 0.2f);
		}
		else
		{
			_camera->GetComponent<PlayerCamera>()->Shake(0.2f, 0.05f);
		}
	}
}

void Hachiko::Scripting::PlayerController::RecieveKnockback(const math::float3 direction)
{
	_state = PlayerState::STUNNED;
	_knock_start = _player_transform->GetGlobalPosition();
	_knock_end = _player_transform->GetGlobalPosition() + direction;
	_stun_time = _stun_duration;
	_player_transform->LookAtDirection(-direction);
}

void Hachiko::Scripting::PlayerController::CheckState()
{
	PlayerState current_state = GetState();
	bool state_changed = current_state != _previous_state;

	if (!state_changed)
	{
		return;
	}

	_previous_state = current_state;

	switch (current_state)
	{
	case PlayerState::IDLE:
		animation->SendTrigger("isIdle");
		break;
	case PlayerState::WALKING:
		animation->SendTrigger("isRun");
		break;
	case PlayerState::PICK_UP:
		animation->SendTrigger("isPickUp");
		break;
	case PlayerState::RANGED_ATTACKING:
		animation->SendTrigger("isShot");
		break;
	case PlayerState::DASHING:
		animation->SendTrigger("isDash");
		break;
	case PlayerState::MELEE_ATTACKING:
		CheckComboAnimation();
		break;
	case PlayerState::FALLING:
		animation->SendTrigger("isFalling");
		break;
	case PlayerState::DIE:
		animation->SendTrigger("isDead");
		break;

	case PlayerState::INVALID:
	default:
		break;
	}
}

// TODO: This is for Alpha, we need to find a better option to trigger this animations
void Hachiko::Scripting::PlayerController::CheckComboAnimation()
{
	if (_current_weapon == 0)
	{
		if (_attack_idx == 0)
		{
			animation->SendTrigger("isMeleeOne");
		}
		else if (_attack_idx == 1)
		{
			animation->SendTrigger("isMeleeTwo");
		}
		else if (_attack_idx == 2)
		{
			animation->SendTrigger("isMeleeThree");
		}
	}
	else if (_current_weapon == 1) // CLAW
	{
		if (_attack_idx == 0)
		{
			animation->SendTrigger("isClawOne");
		}
		else if (_attack_idx == 1)
		{
			animation->SendTrigger("isClawTwo");
		}
		else if (_attack_idx == 2)
		{
			animation->SendTrigger("isClawThree");
		}
	}
	else if (_current_weapon == 2) // SWORD
	{
		if (_attack_idx == 0)
		{
			animation->SendTrigger("isSwordOne");
		}
		else if (_attack_idx == 1)
		{
			animation->SendTrigger("isSwordTwo");
		}
		else if (_attack_idx == 2)
		{
			animation->SendTrigger("isSwordThree");
		}
	}
}

void Hachiko::Scripting::PlayerController::ResetPlayer(float3 spawn_pos)
{
	_player_position = spawn_pos; // _initial_pos;
	_combat_stats->_current_hp = 4;
	UpdateHealthBar();
	// Reset properly
}

void Hachiko::Scripting::PlayerController::UpdateHealthBar()
{
	if (hp_cells.size() < 1)
	{
		HE_LOG("Error. PlayerController is missing Health UI parts");
		return;
	}

	// Disable cells 
	for (int i = 0; i < hp_cells.size(); ++i)
	{
		if (i >= _combat_stats->_current_hp)
		{
			hp_cells[i]->GetComponent(Component::Type::IMAGE)->Disable();
		}
		else
		{
			hp_cells[i]->GetComponent(Component::Type::IMAGE)->Enable();
		}
	}
}

void Hachiko::Scripting::PlayerController::UpdateAmmoUI()
{
	if (ammo_cells.size() < 1)
	{
		HE_LOG("Error. PlayerController is missing Ammo UI references");
		return;
	}

	for(int i = 0; i < ammo_cells.size(); ++i)
	{
		if (i >= _ammo_count)
		{
			ammo_cells[i]->GetComponent(Component::Type::IMAGE)->Disable();
		}
		else
		{
			ammo_cells[i]->GetComponent(Component::Type::IMAGE)->Enable();
		}
	}
}

void Hachiko::Scripting::PlayerController::ToggleGodMode()
{
	_state = PlayerState::IDLE;
	if (!_god_mode)
	{
		HE_LOG("FALLING");
		float3 corrected_position = Navigation::GetCorrectedPosition(_player_position, float3(3.0f, 3.0f, 3.0f));
		if (corrected_position.x < FLT_MAX)
		{
			_player_position = corrected_position;
		}
		// Started falling
		_state = PlayerState::FALLING;
	}
}

void Hachiko::Scripting::PlayerController::CheckGoal(const float3& current_position)
{
	const float3 goal_position = _goal->GetTransform()->GetGlobalPosition();

	if (Distance(current_position, goal_position) < 10.0f)
	{
		//SceneManagement::SwitchScene(12124061992092393469);
	}
}

Hachiko::Scripting::PlayerController::PlayerAttack Hachiko::Scripting::PlayerController::GetAttackType(Hachiko::Scripting::PlayerController::AttackType attack_type)
{
	PlayerAttack attack;
	switch (attack_type)
	{
	// COMMON ATTACKS
	case AttackType::COMMON_1:
		// Make hit delay shorter than duration!
		attack.hit_delay = 0.05f;
		attack.duration = 0.5f; // 10 frames .45ms
		attack.cooldown = 0;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.3f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		break;

	case AttackType::COMMON_2:
		attack.hit_delay = 0.1f;
		attack.duration = 0.40f; // 9 frames .45ms
		attack.cooldown = 0;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.3f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		break;

	case AttackType::COMMON_3:
		attack.hit_delay = 0.2f;
		attack.duration = 0.6f; // 12 frames
		attack.cooldown = 0;
		attack.dash_distance = 1.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 2.f;
		attack.stats.range = 6.f;
		break;

	// COMMON ATTACKS
	case AttackType::QUICK_1:
		attack.hit_delay = 0.05f;
		attack.duration = 0.5f;
		attack.cooldown = 0;
		attack.dash_distance = 1.f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 2.5f;
		break;

	case AttackType::QUICK_2:
		attack.hit_delay = 0.05f;
		attack.duration = 0.40f;
		attack.cooldown = 0;
		attack.dash_distance = 1.f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 2.5f;
		break;

	case AttackType::QUICK_3:
		attack.hit_delay = 0.05f;
		attack.duration = 0.50f;
		attack.cooldown = 0;
		attack.dash_distance = 1.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.f;
		// If its cone use degrees on width
		attack.stats.width = 5.f;
		attack.stats.range = 3.5f;
		break;

	// COMMON ATTACKS
	case AttackType::HEAVY_1:
		attack.hit_delay = 0.1f;
		attack.duration = 0.6f;
		attack.cooldown = 0;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 5.f;
		attack.stats.range = 3.f;
		break;

	case AttackType::HEAVY_2:
		attack.hit_delay = 0.1f;
		attack.duration = 0.4f;
		attack.cooldown = 0;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 5.f;
		attack.stats.range = 3.f;
		break;

	case AttackType::HEAVY_3:
		attack.hit_delay = 0.5f;
		attack.duration = 0.8f;
		attack.cooldown = 0;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 2.f;
		// If its cone use degrees on width
		attack.stats.width = 4.f;
		attack.stats.range = 4.5f;
		break;
	}
	return attack;
}
