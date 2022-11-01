#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"

#include "entities/player/CombatVisualEffectsPool.h"

constexpr int MAX_AMMO = 4;
constexpr int ATTACK_VFX_POOL_SIZE = 6;

Hachiko::Scripting::PlayerController::PlayerController(GameObject* game_object)
	: Script(game_object, "PlayerController")
	, _state(PlayerState::INVALID)
	, _sword_weapon(nullptr)
	, _sword_upper(nullptr)
	, _claw_weapon(nullptr)
	, _hammer_weapon(nullptr)
	, _combat_stats()
	, _attack_indicator(nullptr)
	, _goal(nullptr)
	, _player_geometry(nullptr)
	, _dash_duration(0.0f)
	, _dash_distance(0.0f)
	, _dash_cooldown(0.0f)
	, _invulnerability_time(1.0f)
	, _dash_scaler(3)
	, _max_dash_charges(0)
	, _dash_trail(nullptr)
	, _trail_enlarger(10.0f)
	, _rotation_duration(0.0f)
	, _death_screen(nullptr)
	, _camera(nullptr)
	, _ui_damage(nullptr)
	, _heal_effect_fade_duration(0.3)
	, damage_effect_duration(1.0f)
	, _combat_visual_effects_pool(nullptr)
	, tooltip_y_offset(2.f)
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
	melee.unlimited = true;
	melee.attacks.push_back(GetAttackType(AttackType::COMMON_1));
	melee.attacks.push_back(GetAttackType(AttackType::COMMON_2));
	melee.attacks.push_back(GetAttackType(AttackType::COMMON_3));

	Weapon claw;
	claw.name = "Claw";
	claw.bullet = common_bullet;
	claw.color = float4(0.0f, 0.0f, 255.0f, 255.0f);
	claw.unlimited = false;
	claw.charges = 12;
	claw.attacks.push_back(GetAttackType(AttackType::QUICK_1));
	claw.attacks.push_back(GetAttackType(AttackType::QUICK_2));
	claw.attacks.push_back(GetAttackType(AttackType::QUICK_3));

	Weapon sword;
	sword.name = "Sword";
	sword.bullet = common_bullet;
	sword.color = float4(0.0f, 255.0f, 0.0f, 255.0f);
	sword.unlimited = false;
	sword.charges = 10;
	sword.attacks.push_back(GetAttackType(AttackType::HEAVY_1));
	sword.attacks.push_back(GetAttackType(AttackType::HEAVY_2));
	sword.attacks.push_back(GetAttackType(AttackType::HEAVY_3));

	Weapon hammer;
	hammer.name = "Hammer";
	hammer.bullet = common_bullet;
	hammer.color = float4(0.0f, 255.0f, 255.0f, 0.0f);
	hammer.unlimited = false;
	hammer.charges = 8;
	hammer.attacks.push_back(GetAttackType(AttackType::HAMMER_1));
	hammer.attacks.push_back(GetAttackType(AttackType::HAMMER_2));
	//hammer.attacks.push_back(GetAttackType(AttackType::HAMMER_3));

	weapons.push_back(melee);
	weapons.push_back(claw);
	weapons.push_back(sword);
	weapons.push_back(hammer);

	_current_cam_setting = 0;
}

void Hachiko::Scripting::PlayerController::OnAwake()
{
	_terrain = Scenes::GetTerrainContainer();
	_enemies = Scenes::GetEnemiesContainer();
	_combat_manager = Scenes::GetCombatManager();
	_level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
	_combat_visual_effects_pool = Scenes::GetCombatVisualEffectsPool()->GetComponent<CombatVisualEffectsPool>();

	_dash_charges = _max_dash_charges;

	if (_attack_indicator)
	{
		_attack_indicator->SetActive(false);
		_attack_indicator->SetTimeScaleMode(TimeScaleMode::SCALED);
	}
	if (_ui_damage)
	{
		_ui_damage->SetActive(false);
	}

	if (_dash_trail)
	{
		_dash_trail->SetActive(false);
	}

	if (_dash_trail_vfx)
	{
		_dash_particles = _dash_trail_vfx->GetComponent<ComponentParticleSystem>();
	}

	if (_falling_dust != nullptr) 
	{
		_falling_dust_particles = _falling_dust->GetComponent<ComponentParticleSystem>();
	}
	if (_walking_dust != nullptr)
	{
		_walking_dust_particles = _walking_dust->GetComponent<ComponentParticleSystem>();
	}
	if (_heal_effect != nullptr)
	{
		_heal_effect_particles_1 = _heal_effect->GetComponent<ComponentParticleSystem>();
		_heal_effect_particles_2 = _heal_effect->GetComponentInDescendants<ComponentParticleSystem>();
	}
	if (_damage_effect != nullptr)
	{
		_damage_effect_billboard = _damage_effect->GetComponent<ComponentBillboard>();
	}
	if (_melee_trail_right != nullptr)
	{
		_weapon_trails_billboard_right[static_cast<int>(WeaponUsed::MELEE)] = _melee_trail_right->GetComponent<ComponentBillboard>();
	}
	if (_melee_trail_left != nullptr)
	{
		_weapon_trails_billboard_left[static_cast<int>(WeaponUsed::MELEE)] = _melee_trail_left->GetComponent<ComponentBillboard>();
	}
	if (_melee_trail_center != nullptr)
	{
		_weapon_trails_billboard_center[static_cast<int>(WeaponUsed::MELEE)] = _melee_trail_center->GetComponent<ComponentBillboard>();
	}
	if (_claws_trail_right != nullptr)
	{
		_weapon_trails_billboard_right[static_cast<int>(WeaponUsed::CLAW)] = _claws_trail_right->GetComponent<ComponentBillboard>();
	}
	if (_claws_trail_left != nullptr)
	{
		_weapon_trails_billboard_left[static_cast<int>(WeaponUsed::CLAW)] = _claws_trail_left->GetComponent<ComponentBillboard>();
	}
	if (_sword_trail_right != nullptr)
	{
		_weapon_trails_billboard_right[static_cast<int>(WeaponUsed::SWORD)] = _sword_trail_right->GetComponent<ComponentBillboard>();
	}
	if (_sword_trail_left != nullptr)
	{
		_weapon_trails_billboard_left[static_cast<int>(WeaponUsed::SWORD)] = _sword_trail_left->GetComponent<ComponentBillboard>();
	}
	if (_hammer_trail_right != nullptr)
	{
		_weapon_trails_billboard_right[static_cast<int>(WeaponUsed::HAMMER)] = _hammer_trail_right->GetComponent<ComponentBillboard>();
	}
	if (_hammer_trail_left != nullptr)
	{
		_weapon_trails_billboard_left[static_cast<int>(WeaponUsed::HAMMER)] = _hammer_trail_left->GetComponent<ComponentBillboard>();
	}
	if (_parasite_pickup_effect != nullptr)
	{
		_parasite_pickup_billboard = _parasite_pickup_effect->GetComponent<ComponentBillboard>();
	}
	if (_death_screen != nullptr)
	{
		_death_screen->GetComponent(Component::Type::IMAGE)->Disable();
	}
	
	if (_aim_indicator != nullptr)
	{
		_aim_indicator_billboard = _aim_indicator->GetComponent<ComponentBillboard>();
	}

	_combat_stats = game_object->GetComponent<Stats>();
	// Player doesnt use all combat stats since some depend on weapon
	_combat_stats->_attack_power = 2;
	_combat_stats->_attack_cd = 1;
	_combat_stats->_move_speed = 7.0f;
	_combat_stats->_max_hp = 4;
	_combat_stats->_current_hp = _combat_stats->_max_hp;


	ammo_cells.push_back(_ammo_cell_1);
	ammo_cells.push_back(_ammo_cell_2);
	ammo_cells.push_back(_ammo_cell_3);
	ammo_cells.push_back(_ammo_cell_4);
	_ammo_count = MAX_AMMO;

	_weapon_charge_bar = _weapon_charge_bar_go->GetComponent<ComponentProgressBar>();

	// The first position will be the current position the camera has
	_cam_positions.push_back(float3(0.0f, 20.0f, 13.0f));
	_cam_rotations.push_back(float3(125.0f, 0.0f, 180.0f));

	// Second position is the "Basic" position for travelling camera
	_cam_positions.push_back(float3(0.0f, 26.0f, 17.5f));
	_cam_rotations.push_back(float3(125.0f, 0.0f, 180.0f));

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

	ChangeWeapon(0);
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

	_aim_indicator_billboard->Stop();

	if (_level_manager->increased_health)
	{
		IncreaseHealth();
	}

	UpdateHealthBar();

}

void Hachiko::Scripting::PlayerController::OnUpdate()
{
	_player_transform = game_object->GetTransform();
	_player_position = _player_transform->GetGlobalPosition();
	_movement_direction = float3::zero;

	_lock_time -= Time::DeltaTimeScaled();
	if (_lock_time < 0.0f)
	{
		_lock_time = 0.0f;
	}

	UpdateVignete();

	UpdateEmissives();

	if (_enable_heal_particles && _heal_fade_progress < 0.0f) {
		_heal_effect_particles_1->Restart();
		_heal_effect_particles_2->Restart();
		_enable_heal_particles = false;
	}
		
	
	if (_invulnerability_time_remaining > 0.0f)
	{
		_invulnerability_time_remaining -= Time::DeltaTimeScaled();
		if (_invulnerability_time_remaining <= 0.0f && _player_geometry != nullptr)
		{
			_player_geometry->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, 1.0f), true);
		}
	}

	if (_god_mode_trigger)
	{
		_god_mode = !_god_mode;
		ToggleGodMode();
		_god_mode_trigger = false;
	}

	if (IsAlive())
	{
		if (!_level_manager->AreInputsBlocked())
		{
			// Handle player the input
			HandleInputAndStatus();
		}

		// Run attack simulation
		AttackController();

		// Run movement simulation
		MovementController();

		CheckGoal(_player_position);

		// Rotate player to the necessary direction:
		WalkingOrientationController();

		CheckNearbyParasytes(_player_position);

		// Apply the position after everything is simulated
		_player_transform->SetGlobalPosition(_player_position);
	}
	else
	{
		if (_state == PlayerState::READY_TO_RESPAWN)
		{
			if (Input::IsKeyPressed(Input::KeyCode::KEY_R) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_Y))
			{
				if (_death_screen != nullptr)
				{
					_death_screen->GetComponent(Component::Type::IMAGE)->Disable();
				}
				if (_level_manager->_level > 2) {
					_level_manager->ReloadBossScene();
				}

				ResetPlayer(_level_manager->Respawn());

				// Apply the position after the current position is set to the respawn position
				_player_transform->SetGlobalPosition(_player_position);
			}
		}
		else
		{
			_state = PlayerState::DIE;

			// Some basic resets as disabling the dash and restarting the particles
			_dash_trail->SetActive(false);
			StopParticles();

			// Close up to player when dying
			if (!_is_dying)
			{
				_camera->GetComponent<PlayerCamera>()->ChangeRelativePosition(float3(0.0, 10.0, 7.0), false, 0.005f, 0.0f, false);
				_is_dying = true;
			}

			// By checking previous state we know that the current animation is the correct one
			if (_previous_state == PlayerState::DIE && animation->IsAnimationStopped())
			{
				_state = PlayerState::READY_TO_RESPAWN;
				damaged_by = DamageType::NONE;

				if (_death_screen != nullptr)
				{
					_death_screen->GetComponent(Component::Type::IMAGE)->Enable();
				}
			}
		}
	}

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

	if (Input::IsGamepadModeOn())
	{
		const math::float2 gamepad_normalized_position =
			math::float2(Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_LEFTX), Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_LEFTY));

		if (gamepad_normalized_position.x <= 0.2 && gamepad_normalized_position.x >= -0.2 && gamepad_normalized_position.y <= 0.2 && gamepad_normalized_position.y >= -0.2)
		{
			return current_position + _player_transform->GetFront();
		}

		const math::float2 gamepad_position_view =
			ComponentCamera::ScreenPositionToView(gamepad_normalized_position);

		const math::LineSegment ray = Debug::GetRenderingCamera()->Raycast(
			gamepad_position_view.x, gamepad_position_view.y);

		return plane.ClosestPoint(ray);
	}

	const math::float2 mouse_position_view =
		ComponentCamera::ScreenPositionToView(Input::GetMouseNormalizedPosition());

	const math::LineSegment ray = Debug::GetRenderingCamera()->Raycast(
		mouse_position_view.x, mouse_position_view.y);

	return plane.ClosestPoint(ray);
}

void Hachiko::Scripting::PlayerController::ActivateTooltip(const float3& position)
{
	float3 final_position = position;
	final_position.y += tooltip_y_offset;

	if (Input::IsGamepadModeOn())
	{
		_controller_tooltip_display->GetTransform()->SetGlobalPosition(final_position);
		_controller_tooltip_display->GetComponent(Component::Type::BILLBOARD)->Start();
	}
	else
	{
		_keyboard_tooltip_display->GetTransform()->SetGlobalPosition(final_position);
		_keyboard_tooltip_display->GetComponent(Component::Type::BILLBOARD)->Start();
	}
}

void Hachiko::Scripting::PlayerController::DeactivateTooltip()
{
	_controller_tooltip_display->GetComponent(Component::Type::BILLBOARD)->Stop();
	_keyboard_tooltip_display->GetComponent(Component::Type::BILLBOARD)->Stop();
}

float3 Hachiko::Scripting::PlayerController::GetCorrectedPosition(const float3& target_pos, bool fps_relative) const
{
	if (fps_relative)
	{
		const float correction_distance = Time::DeltaTime() * _combat_stats->_move_speed + 0.05;
		const float y_correction_distance = Time::DeltaTime() * fall_speed + 0.2;
		return Navigation::GetCorrectedPosition(target_pos, float3(correction_distance, y_correction_distance, correction_distance));
	}
	return Navigation::GetCorrectedPosition(target_pos, float3(0.5f, 0.5f, 0.5f));
}

void Hachiko::Scripting::PlayerController::SpawnGameObject() const
{
	static int times_hit_g = 0;

	if (!Input::IsKeyDown(Input::KeyCode::KEY_G) || !(Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_BACK) && Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_START)))
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
	// Buffered click remaining time
	_remaining_buffer_time -= Time::DeltaTime();
	if (_remaining_buffer_time < 0.0f)
	{
		_remaining_buffer_time = 0.0f;
	}

	// Movement Direction
	if (Input::IsKeyPressed(Input::KeyCode::KEY_W) || Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_LEFTY) < 0)
	{
		_movement_direction -= math::float3::unitZ;
	}
	else if (Input::IsKeyPressed(Input::KeyCode::KEY_S) || Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_LEFTY) > 0)
	{
		_movement_direction += math::float3::unitZ;
	}

	if (Input::IsKeyPressed(Input::KeyCode::KEY_D) || Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_LEFTX) > 0)
	{
		_movement_direction += math::float3::unitX;
	}
	else if (Input::IsKeyPressed(Input::KeyCode::KEY_A) || Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_LEFTX) < 0)
	{
		_movement_direction -= math::float3::unitX;
	}

	if (!IsActionLocked())
	{
		if (!IsAttackOnCooldown() && dash_buffer && _dash_charges > 0)
		{
			dash_buffer = false;
			_remaining_buffer_time = 0.0f;
			Dash();
		}
		else if (!IsAttackOnCooldown() && (HasBufferedClick() || Input::IsMouseButtonDown(Input::MouseButton::LEFT) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_X)))
		{
			MeleeAttack();
		}
		else if (!IsAttackOnCooldown() && _ammo_count > 0 && (Input::IsMouseButtonDown(Input::MouseButton::RIGHT) || (Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_LEFTSHOULDER) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_RIGHTSHOULDER))))
		{
			RangedAttack();
		}
		// Keep dash here since it uses the input movement direction
		else if ((Input::IsKeyDown(Input::KeyCode::KEY_SPACE) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_A)) && _dash_charges > 0)
		{
			Dash();
		}
		else if (!_movement_direction.Equals(float3::zero))
		{
			_state = PlayerState::WALKING;
		}
		else if (!IsActionOnProgress())
		{
			_state = PlayerState::IDLE;
		}
	}	
	else
	{
		HandleInputBuffering();
	}

	// Range attack charge cancel
	if (Input::IsMouseButtonUp(Input::MouseButton::RIGHT) ||
		Input::IsGameControllerButtonUp(Input::GameControllerButton::CONTROLLER_BUTTON_LEFTSHOULDER) ||
		Input::IsGameControllerButtonUp(Input::GameControllerButton::CONTROLLER_BUTTON_RIGHTSHOULDER))
	{
		ReleaseAttack();
	}

	if (_god_mode && (Input::IsKeyPressed(Input::KeyCode::KEY_Q) || Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_TRIGGERLEFT) > 0))
	{
		_player_position += math::float3::unitY * 0.5f;
	}
	else if (_god_mode && (Input::IsKeyPressed(Input::KeyCode::KEY_E) || Input::GetAxisNormalized(Input::GameControllerAxis::CONTROLLER_AXIS_TRIGGERRIGHT) > 0))
	{
		_player_position -= math::float3::unitY * 0.5f;
	}

	if (Input::IsKeyDown(Input::KeyCode::KEY_G) || (Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_BACK) && Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_START)))
	{
		_god_mode = !_god_mode;

		ToggleGodMode();
	}
}

void Hachiko::Scripting::PlayerController::HandleInputBuffering()
{
	if (Input::IsMouseButtonDown(Input::MouseButton::LEFT) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_X))
	{
		// Melee combo input buffer
		click_buffer = GetRaycastPosition(_player_position);
		_remaining_buffer_time = _buffer_time;
	}
	if (Input::IsKeyDown(Input::KeyCode::KEY_SPACE) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_A))
	{
		// Dash to cut a combo
		CancelAttack();
		dash_buffer = true;
	}
}

bool Hachiko::Scripting::PlayerController::HasBufferedClick()
{
	return _remaining_buffer_time > 0.0f;
}

float3 Hachiko::Scripting::PlayerController::GetBufferedClick()
{
	if (_remaining_buffer_time < 0.0f)
	{
		return float3::zero;
	}

	float3 direction = click_buffer;
	_remaining_buffer_time = 0.0f;

	return direction;
}

void Hachiko::Scripting::PlayerController::Dash()
{
	_state = PlayerState::DASHING;
	_dash_charges -= 1;
	_dash_progress = 0.f;
	_dash_start = _player_position;
	_current_dash_duration = _dash_duration;
	_lock_time = _dash_duration;

	StoreDashOrigin(_dash_start);

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
	_player_transform->LookAtTarget(_dash_end);

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
		if (corrected_dash_destination.x >= FLT_MAX)
		{
			_player_position = GetLastValidDashOrigin();
			if (_player_position.x >= FLT_MAX)
			{
				// If there is no valid position send it to the last checkpoint
				_player_position = _level_manager->Respawn();
			}
		}
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

void Hachiko::Scripting::PlayerController::StoreDashOrigin(const float3& dash_origin)
{
	dashed_from_positions.push_back(dash_origin);
	while (dashed_from_positions.size() > max_dashed_from_positions)
	{
		dashed_from_positions.pop_front();
	}
}

float3 Hachiko::Scripting::PlayerController::GetLastValidDashOrigin()
{
	float3 valid_pos(FLT_MAX);

	while (!dashed_from_positions.empty())
	{
		valid_pos = GetCorrectedPosition(dashed_from_positions.back());
		if (valid_pos.x < FLT_MAX)
		{
			return valid_pos;
		}
		dashed_from_positions.pop_back();
	}
	return valid_pos;
}

void Hachiko::Scripting::PlayerController::MeleeAttack()
{
	_state = PlayerState::MELEE_ATTACKING;
	Weapon& weapon = GetCurrentWeapon();
	const PlayerAttack& attack = GetNextAttack();
	switch (attack.stats.attack_trail)
	{
	case CombatManager::AttackTrail::RIGHT:
		_weapon_trails_billboard_right[static_cast<int>(GetCurrentWeaponType())]->Play();
		break;
	case CombatManager::AttackTrail::LEFT:
		_weapon_trails_billboard_left[static_cast<int>(GetCurrentWeaponType())]->Play();
		break;
	case CombatManager::AttackTrail::CENTER:
		_weapon_trails_billboard_center[static_cast<int>(GetCurrentWeaponType())]->Play();
		break;
	}
	if (!weapon.unlimited && _attack_charges)
	{
		_attack_charges--;
	}
	UpdateWeaponChargeUI();

	_attack_current_duration = attack.duration;
	_after_attack_timer = attack.cooldown + _combo_grace_period;

	// The attacks lock the player for its duration
	_lock_time = attack.duration;

	// Attack will occur in the attack simulation after the delay
	_attack_current_delay = attack.hit_delay;

	if (!Input::IsGamepadModeOn())
	{
		float3 direction = HasBufferedClick() ? GetBufferedClick() : GetRaycastPosition(_player_position);
		_player_transform->LookAtTarget(direction);
	}
	else
	{
		GetBufferedClick();
	}

	// Move player a bit forward if it wouldnt fall	
	if (attack.dash_distance != 0.0f)
	{
		_dash_progress = 0.f;
		_current_dash_duration = attack.duration * (2 / 3);
		_dash_start = _player_position;

		// Check for inertia
		float inertia_value = 0;
		if (_previous_state == PlayerState::WALKING)
		{
			inertia_value = _movement_direction.Normalized().Dot(_player_transform->GetFront().Normalized());
			inertia_value = math::Clamp(inertia_value, 0.0f, 1.0f);
		}

		_dash_end = _player_position + _player_transform->GetFront().Normalized() * attack.dash_distance * (1 + inertia_value);

		// Instead of using "CorrectDashDestination(_dash_start, _dash_end);", since its an attack dash use a greater correction 
		float3 corrected_dash_destination = Navigation::GetCorrectedPosition(_dash_end, float3(1.0f, 0.5f, 1.0f));
		if (corrected_dash_destination.x < FLT_MAX)
		{
			_dash_end = corrected_dash_destination;
		}
		else
		{
			_dash_end = _dash_start;
		}
	}
	_new_attack = true;
	_request_attack_sound = true;

	// Fast and Scuffed, has to be changed when changing attack indicator
	float4 attack_color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	_attack_indicator->ChangeEmissiveColor(attack_color, true);
}

void Hachiko::Scripting::PlayerController::ChangeWeapon(unsigned weapon_idx)
{
	_current_weapon = weapon_idx;

	if (weapon_idx >= weapons.size())
	{
		_current_weapon = 0;
	}
	_attack_charges = GetCurrentWeapon().charges;
	_weapon_charge_bar->SetMax(_attack_charges);
	_weapon_charge_bar->SetMin(0);

	UpdateWeaponChargeUI();

	if (_current_weapon == 0) { // MELEE
		_claw_weapon->SetActive(false);
		_sword_upper->SetActive(false);
		_sword_weapon->SetActive(false);
		_hammer_weapon->SetActive(false);

		_sword_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
		_claw_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
		_maze_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
	}
	else if (_current_weapon == 1) // CLAW
	{
		_claw_weapon->SetActive(true);
		_sword_upper->SetActive(false);
		_sword_weapon->SetActive(false);
		_hammer_weapon->SetActive(false);

		_sword_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
		_claw_ui_addon->GetComponent(Component::Type::IMAGE)->Enable();
		_maze_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
	}
	else if (_current_weapon == 2) // SWORD
	{
		_claw_weapon->SetActive(false);
		_sword_upper->SetActive(true);
		_sword_weapon->SetActive(true);
		_hammer_weapon->SetActive(false);

		_sword_ui_addon->GetComponent(Component::Type::IMAGE)->Enable();
		_claw_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
		_maze_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
	}
	else if (_current_weapon == 3) // HAMMER
	{
		_claw_weapon->SetActive(false);
		_sword_upper->SetActive(false);
		_sword_weapon->SetActive(false);
		_hammer_weapon->SetActive(true);

		_sword_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
		_claw_ui_addon->GetComponent(Component::Type::IMAGE)->Disable();
		_maze_ui_addon->GetComponent(Component::Type::IMAGE)->Enable();
	}
	_parasite_pickup_billboard->Play();
}

void Hachiko::Scripting::PlayerController::ReloadAmmo(unsigned ammo)
{
	_ammo_count += ammo;
	if (_ammo_count > MAX_AMMO)
	{
		_ammo_count = MAX_AMMO;
	}
	UpdateAmmoUI();
}

bool Hachiko::Scripting::PlayerController::IsAttacking() const
{
	return _state == PlayerState::MELEE_ATTACKING || _state == PlayerState::RANGED_ATTACKING || _state == PlayerState::RANGED_CHARGING;
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
	return _lock_time > 0.0f || IsStunned() || IsFalling() || IsDying();
}

bool Hachiko::Scripting::PlayerController::IsActionOnProgress() const
{
	return IsAttacking() || IsDashing() || IsPickUp() || IsStunned() || IsFalling() || IsDying();
}

bool Hachiko::Scripting::PlayerController::IsAttackOnCooldown() const
{
	return _after_attack_timer - _combo_grace_period > 0;
}

bool Hachiko::Scripting::PlayerController::IsInComboWindow() const
{
	return _after_attack_timer > 0;
}

Hachiko::Scripting::PlayerController::Weapon& Hachiko::Scripting::PlayerController::GetCurrentWeapon()
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
			_remaining_buffer_time = 0.0f;
			_attack_idx = 0;
		}
	}
	else
	{
		_attack_idx = 0;
	}

	return GetCurrentAttack();
}

const Hachiko::Scripting::PlayerController::PlayerAttack& Hachiko::Scripting::PlayerController::GetCurrentAttack()
{
	return GetCurrentWeapon().attacks[_attack_idx];
}

void Hachiko::Scripting::PlayerController::RangedAttack()
{
	_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
	const float3 forward = _player_transform->GetFront().Normalized();

	CombatManager* bullet_controller = _combat_manager->GetComponent<CombatManager>();
	if (bullet_controller)
	{
		CombatManager::BulletStats stats = GetCurrentWeapon().bullet;
		_attack_current_duration = 0.f;
		_lock_time = 999.f;
		_current_bullet = bullet_controller->ChargeBullet(_player_transform, stats);
		if (_current_bullet >= 0)
		{
			math::Clamp(--_ammo_count, 0, MAX_AMMO);
			_state = PlayerState::RANGED_CHARGING;
		}
	}

	_aim_indicator_billboard->Restart();
}

void Hachiko::Scripting::PlayerController::ReleaseAttack()
{
	_lock_time = 0.0f;

	if (_state == PlayerState::RANGED_CHARGING && _current_bullet >= 0)
	{
		CombatManager* bullet_controller = _combat_manager->GetComponent<CombatManager>();
		if (bullet_controller->ShootBullet(_current_bullet))
		{
			_attack_current_duration = 0.4f;
			_state = PlayerState::RANGED_ATTACKING;
		}
		else
		{
			math::Clamp(++_ammo_count, 0, MAX_AMMO);
			_state = PlayerState::IDLE;
		}

		_aim_indicator_billboard->Stop();
	}
}

void Hachiko::Scripting::PlayerController::CancelAttack()
{
	// Indirectly cancells atack by putting its remaining duration to 0
	_attack_current_duration = 0.f;
	_lock_time = 0.0f;

	if (_state == PlayerState::RANGED_CHARGING && _current_bullet >= 0)
	{
		CombatManager* bullet_controller = _combat_manager->GetComponent<CombatManager>();
		bullet_controller->StopBullet(_current_bullet);
		math::Clamp(++_ammo_count, 0, MAX_AMMO);

		_state = PlayerState::IDLE;
		_aim_indicator_billboard->Stop();
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
	GameObject* terrain_hit = SceneManagement::RayCast(start, end, &collision_point, _terrain, true);
	return terrain_hit != nullptr;
}

void Hachiko::Scripting::PlayerController::MovementController()
{
	DashController();
	WalkingOrientationController();

	if (IsWalking())
	{
		_player_position += (_movement_direction * _combat_stats->_move_speed * Time::DeltaTime());
		if (_walking_dust_particles)
			_walking_dust_particles->Play();
	}
	else
	{
		if (_walking_dust_particles)
			_walking_dust_particles->Stop();
	}

	if (_god_mode)
	{
		return;
	}

	if (IsFalling())
	{
		_player_position.y -= fall_speed * Time::DeltaTimeScaled();

		if (_start_fall_pos.y - _player_position.y > _falling_distance)
		{
			// Fall dmg
			RegisterHit(1, 0, float3::zero, true, DamageType::FALL);

			// If its still alive place it in the first valid position, if none exists respawn it
			_player_position = GetLastValidDashOrigin();
			if (_player_position.x >= FLT_MAX)
			{
				// If there is no valid position send it to the last checkpoint
				_player_position = _level_manager->Respawn();
			}

			// Check if the player has survived
			if (IsAlive())
			{
				_state = PlayerState::IDLE;
			}
			else
			{
				return;
			}
		}
	}
	else if (IsStunned())
	{
		if (_stun_time > 0.0f)
		{
			_stun_time -= Time::DeltaTimeScaled();
			float stun_completion = (_stun_duration - _stun_time) * (1.0 / _stun_duration);
			_player_position = math::float3::Lerp(_knock_start, _knock_end,
				stun_completion);
			_state = PlayerState::STUNNED;
		}
		else
		{
			_player_position = GetCorrectedPosition(_knock_end);
			if (_player_position.x >= FLT_MAX)
			{
				_player_position = _knock_end;
			}
			_state = PlayerState::IDLE;
		}
	}

	float3 corrected_position = GetCorrectedPosition(_player_position, true);
	// Valid corrected position
	if (corrected_position.x < FLT_MAX)
	{
		if (IsFalling())
		{
			if (corrected_position.y > _player_position.y)
			{
				_player_position = corrected_position;

				// Stopped falling
				_state = PlayerState::IDLE;
				if (_falling_dust_particles)
					_falling_dust_particles->Restart();
			}
		}
		else
		{
			_player_position = corrected_position;
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
		DashTrailManager(_dash_progress);
		return;
	}

	if (_dash_particles != nullptr && IsDashing())
	{
		_dash_particles->Play();
	}

	_dash_progress += Time::DeltaTime() / _dash_duration;
	_dash_progress = _dash_progress > 1.0f ? 1.0f : _dash_progress;

	// using y = x^p
	float acceleration = 1.0f - math::Pow((1.0f - _dash_progress) / 1.0f, (int)_dash_scaler);

	_player_position = math::float3::Lerp(_dash_start, _dash_end,
		acceleration);


	DashTrailManager(_dash_progress);
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

	_dash_charging_time += Time::DeltaTimeScaled();

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
	_show_dashtrail = _state == PlayerState::DASHING;
	_dash_trail->SetActive(_show_dashtrail);

	if (!_show_dashtrail)
	{
		return;
	}
	_dash_trail->GetTransform()->SetLocalPosition(math::float3::Lerp(_trail_start_pos, _trail_end_pos,
		_dash_progress));
	_dash_trail->GetTransform()->SetLocalScale(math::float3::Lerp(_trail_start_scale, _trail_end_scale,
		_dash_progress));
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
			_after_attack_timer -= Time::DeltaTimeScaled();
		}
		return;
	}

	if (_attack_current_duration > 0.0f)
	{
		_attack_current_duration -= Time::DeltaTimeScaled();

		if (_state == PlayerState::MELEE_ATTACKING)
		{
			const PlayerAttack& attack = GetCurrentAttack();
			const Weapon& weapon = GetCurrentWeapon();
			CombatManager* combat_manager = _combat_manager->GetComponent<CombatManager>();

//// UNCOMMENT THIS SECTION IF YOU WANT TO DEBUG DRAW ATTACK HIT BOXES
//if (attack.stats.type == CombatManager::AttackType::CONE)
//{
//	_attack_indicator->SetActive(true);
//}
//else
//{
//	if (combat_manager)
//	{
//		Debug::DebugDraw(combat_manager->CreateAttackHitbox(GetMeleeAttackOrigin(attack.stats.range), attack.stats), weapon.color.Float3Part());
//	}
//}

if (_attack_current_delay > 0.f)
{
	_attack_current_delay -= Time::DeltaTimeScaled();

	if (_attack_current_delay <= 0.f)
	{
		int hit_count = 0;

		if (combat_manager)
		{
			// Offset the center of the attack if its a rectangle
			if (attack.stats.type == CombatManager::AttackType::RECTANGLE)
			{
				hit_count = combat_manager->PlayerMeleeAttack(GetMeleeAttackOrigin(attack.stats.range), attack.stats);
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
		}
	}
}
		}
	}

	if (_state == PlayerState::RANGED_CHARGING)
	{
		_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
	}

	Weapon& weapon = GetCurrentWeapon();
	if (!weapon.unlimited && !_attack_charges)
	{
		ChangeWeapon(0);
	}

	if (_attack_current_duration <= 0)
	{
		if (_state == PlayerState::RANGED_CHARGING)
		{
			return;
		}

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

void Hachiko::Scripting::PlayerController::CheckNearbyParasytes(const float3& current_position)
{
	GameObject* closest_parasyte_in_range = nullptr;
	float closest_parasyte_distance = FLOAT_INF;
	
	if (_enemies == nullptr)
	{
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

		float parasyte_pickup_distance = 3.5f;

		if (_magic_parasyte && _magic_parasyte->IsActive())
		{
			if (parasyte_pickup_distance >= _player_transform->GetGlobalPosition().Distance(_magic_parasyte->GetTransform()->GetGlobalPosition()))
			{
				// If there is a nearby parasyte tooltip of the normal parasyte would be the one appearing
				// This will never happen on our level layout so its fine
				ActivateTooltip(_magic_parasyte->GetTransform()->GetGlobalPosition());
				if (Input::IsKeyDown(Input::KeyCode::KEY_F) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_B))
				{
					PickupParasite(nullptr, true);
					_magic_parasyte->SetActive(false);
					DeactivateTooltip();
				}
				return;
			}
		}

		for (int i = 0; i < enemies.size(); ++i)
		{
			if (enemies[i]->active && parasyte_pickup_distance >= _player_transform->GetGlobalPosition().Distance(enemies[i]->GetTransform()->GetGlobalPosition()))
			{
				EnemyController* enemy_controller = enemies[i]->GetComponent<EnemyController>();

				if (!enemy_controller->IsAlive() && enemy_controller->ParasiteDropped())
				{
					if (!closest_parasyte_in_range)
					{
						closest_parasyte_in_range = enemies[i];
						closest_parasyte_distance = Distance(_player_position, closest_parasyte_in_range->GetTransform()->GetGlobalPosition());
					}
					else
					{
						float dist = Distance(_player_position, closest_parasyte_in_range->GetTransform()->GetGlobalPosition());
						if (dist < closest_parasyte_distance)
						{
							closest_parasyte_in_range = enemies[i];
						}
					}
					
					
					ActivateTooltip(closest_parasyte_in_range->GetTransform()->GetGlobalPosition());

					if (Input::IsKeyDown(Input::KeyCode::KEY_F) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_B))
					{
						PickupParasite(enemy_controller);
						DeactivateTooltip();
					}
					return;
				}
			}
			else
			{
				continue;
			}
		}
	}

	DeactivateTooltip();
}

void Hachiko::Scripting::PlayerController::PickupParasite(EnemyController* enemy_controller, bool magic_parasyte)
{
	_state = PlayerState::PICK_UP;

	if (enemy_controller)
	{
		enemy_controller->GetParasite();
	}
	

	// Make player invulnerable for a period of time:
	_invulnerability_time_remaining = _invulnerability_time;

	_combat_stats->Heal(1);

	_heal_effect_particles_1->Restart();
	_heal_effect_particles_2->Restart();
	_heal_fade_progress = 1.0f;
	_enable_heal_particles = true;

	if (magic_parasyte)
	{
		IncreaseHealth();
	}

	UpdateHealthBar();

	if (_ui_damage && _ui_damage->IsActive())
	{
		_lh_vfx_current_time = 0.0f;
		_ui_damage->GetComponent<ComponentImage>()->SetColor(float4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	// Select a random weapon:
	if (!magic_parasyte)
	{
		ChangeWeapon(RandomUtil::RandomIntBetween(1, weapons.size() - 1));
	}
}

bool Hachiko::Scripting::PlayerController::RegisterHit(int damage_received, float knockback, float3 direction, bool force_dmg, DamageType dmg_by)
{
	damaged_by = dmg_by;

	if (_god_mode || !IsAlive() || _level_manager->AreInputsBlocked())
	{
		return false;
	}

	bool dmg_received = _invulnerability_time_remaining <= 0.0f;
	if (dmg_received || force_dmg)
	{
		_invulnerability_time_remaining = _invulnerability_time;
		if (_player_geometry != nullptr)
		{
			_player_geometry->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, 0.5f), true);
		}

		_combat_stats->ReceiveDamage(damage_received);
		UpdateHealthBar();
		Input::GoBrr(0.3f, 500);

		if (_player_geometry != nullptr)
		{
			damage_effect_progress = damage_effect_duration;
		}

		// Activate vignette
		if (_ui_damage && _combat_stats->_current_hp <= 1)
		{
			_lh_vfx_current_time = 0.0f;
			_ui_damage->SetActive(true);
			_ui_damage->GetComponent<ComponentImage>()->SetColor(float4(1.0f, 1.0f, 1.0f, 0.0f));
		}

		if (_damage_effect_billboard != nullptr)
		{
			_damage_effect_billboard->Play();
		}
	}

	if (knockback > 0.0f)
	{
		if (IsDashing())
		{
			_state = PlayerState::IDLE;
		}
		RecieveKnockback(direction * knockback);
		_camera->GetComponent<PlayerCamera>()->Shake(0.5f, 0.8f);
	}
	else
	{
		if (_combat_stats->_current_hp <= 1)
		{
			_camera->GetComponent<PlayerCamera>()->Shake(_low_health_vfx_time, 0.8f);
		}
		else
		{
			_camera->GetComponent<PlayerCamera>()->Shake(0.2f, 0.5f);
		}
	}
	return dmg_received;
}

void Hachiko::Scripting::PlayerController::RecieveKnockback(const math::float3 direction)
{
	_state = PlayerState::STUNNED;
	_knock_start = _player_transform->GetGlobalPosition();
	_knock_end = Navigation::GetCorrectedPosition(_player_transform->GetGlobalPosition() + direction, float3(10.0f, 10.0f, 10.0f));
	if (_knock_end.x >= FLT_MAX)
	{
		_knock_end = _knock_start;
	}
	_stun_time = _stun_duration;
	//_player_transform->LookAtDirection(-direction);
}

void Hachiko::Scripting::PlayerController::CheckState()
{
	PlayerState current_state = GetState();
	bool state_changed = current_state != _previous_state;

	// Check also if another attack is made
	if (!state_changed && !_new_attack)
	{
		return;
	}

	switch (current_state)
	{
	case PlayerState::IDLE:
		if (_previous_state == PlayerState::READY_TO_RESPAWN)
		{
			animation->SendTrigger("isRespawn");
		}
		else
		{
			animation->SendTrigger("isIdle");
		}
		break;
	case PlayerState::WALKING:
		animation->SendTrigger("isRun");
		break;
	case PlayerState::STUNNED:
		animation->SendTrigger("isWounded");
		break;
	case PlayerState::PICK_UP:
		animation->SendTrigger("isPickUp");
		break;
	case PlayerState::RANGED_CHARGING:
		animation->SendTrigger("isCharge");
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

	_previous_state = current_state;
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
	else if (_current_weapon == 3) // HAMMER
	{
		if (_attack_idx == 0)
		{
			animation->SendTrigger("isHammerOne");
		}
		else if (_attack_idx == 1)
		{
			animation->SendTrigger("isHammerTwo");
		}
		else if (_attack_idx == 2)
		{
			animation->SendTrigger("isHammerThree");
		}
	}
	_new_attack = false;
}

void Hachiko::Scripting::PlayerController::ResetPlayer(float3 spawn_pos)
{
	_player_position = spawn_pos; // _initial_pos;
	_combat_stats->_current_hp = 4;

	// Reset properly

	_ammo_count = 4;

	_remaining_buffer_time = 0.0f;
	dash_buffer = false;

	_dash_charges = 2;
	_current_dash_duration = 0.f;
	_dash_progress = 0.0f;
	_dash_charging_time = 0.0f;
	_dash_start = float3::zero;
	_dash_end = float3::zero;
	_dash_direction = float3::zero;

	// Dash
	_dash_trail->SetActive(false);
	_show_dashtrail = false;

	// Attack management
	_attack_current_cd = 0.0f;
	_attack_current_duration = 0.0f;
	_current_attack_cooldown = 0.f;
	_attack_current_delay = 0.0f;
	_after_attack_timer = 0.f;;
	_current_bullet = -1;
	_attack_idx = 0;
	_current_weapon = 0;
	_invulnerability_time_remaining = 0.0f;

	// Movement management
	_stun_time = 0.0f;
	_stun_duration = 0.5f;
	_rotation_progress = 0.0f;
	_falling_distance = 10.0f;
	_should_rotate = false;
	_is_falling = false;

	_lock_time = 0.0f;

	// Camera
	_current_cam_setting = 0;

	_movement_direction = float3::zero;
	_knock_start = float3::zero;
	_knock_end = float3::zero;
	_start_fall_pos = float3::zero;
	_rotation_start = Quat::identity;
	_rotation_target = Quat::identity;

	// Color
	_player_geometry->ChangeTintColor(float4(1.0f, 1.0f, 1.0f, 1.0f), true);

	// Particles
	StopParticles();

	// State
	_state = PlayerState::IDLE;

	_aim_indicator_billboard->Stop();

	_ui_damage->SetActive(false);
	_camera->GetComponent<PlayerCamera>()->ChangeRelativePosition(_cam_positions[1], true, 0.0f);
	_is_dying = false;

	// Camera gets set back to player when it respawns
	_camera->GetComponent<PlayerCamera>()->ChangeRelativePosition(_cam_positions[1], false, .2f);
	_camera->GetComponent<PlayerCamera>()->RotateCameraTo(_cam_rotations[0], .2f);
	_camera->GetComponent<PlayerCamera>()->SetObjective(game_object);

	ChangeWeapon(_current_weapon);
	UpdateHealthBar();
	UpdateAmmoUI();
}

void Hachiko::Scripting::PlayerController::StopParticles()
{
	if (_falling_dust_particles)
	{
		_falling_dust_particles->Stop();
	}
	if (_walking_dust_particles)
	{
		_walking_dust_particles->Stop();
	}
	if (_heal_effect_particles_1)
	{
		_heal_effect_particles_1->Stop();
	}
	if (_heal_effect_particles_2)
	{
		_heal_effect_particles_2->Stop();
	}
}

void Hachiko::Scripting::PlayerController::UpdateHealthBar()
{
	if (hp_cells.size() < 1)
	{
		HE_LOG("Error. PlayerController is missing Health UI parts");
		return;
	}

	if (hp_cells.size() < 5 && _hp_cell_extra)
	{
		_hp_cell_extra->GetComponent(Component::Type::IMAGE)->Disable();
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

	for (int i = 0; i < ammo_cells.size(); ++i)
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

void Hachiko::Scripting::PlayerController::UpdateWeaponChargeUI()
{
	Weapon& weapon = GetCurrentWeapon();
	if (weapon.unlimited)
	{
		_weapon_charge_bar_go->SetActive(false);
		_weapon_charge_bar->Disable();
		return;
	}
	_weapon_charge_bar_go->SetActive(true);
	_weapon_charge_bar->Enable();
	_weapon_charge_bar->SetFilledValue(_attack_charges);
}

void Hachiko::Scripting::PlayerController::ToggleGodMode()
{
	_state = PlayerState::IDLE;
	if (!_god_mode)
	{
		_dash_start = _player_position;
		float3 corrected_position = Navigation::GetCorrectedPosition(_player_position, float3(1500.0f, 1500.0f, 1500.0f));
		if (corrected_position.x < FLT_MAX)
		{
			_player_position = corrected_position;
		}
		else
		{
			// Started falling
			_state = PlayerState::FALLING;
			_player_position = float3::zero;
		}
	}
}

void Hachiko::Scripting::PlayerController::CheckGoal(const float3& current_position)
{
	const float3 goal_position = _goal->GetTransform()->GetGlobalPosition();

	if (Distance(current_position, goal_position) < 10.0f)
	{
		_level_manager->GoalReached();
	}
}

void Hachiko::Scripting::PlayerController::UpdateVignete()
{
	if (!_ui_damage || !_ui_damage->IsActive())
	{
		return;
	}

	float transparency = 0.0f;
	float progress = _lh_vfx_current_time / _low_health_vfx_time;
	float clamp_max = 1.0f;

	switch (_combat_stats->_current_hp)
	{
	case 0:
		clamp_max = 3.0f;
		transparency = math::Clamp(progress, 0.0f, clamp_max);;
		break;
	case 1:
		clamp_max = 1.0f;
		transparency = math::Clamp(progress, 0.0f, clamp_max);;
		break;
	default:
		clamp_max = 1.0f;
		transparency = math::Clamp(1 - progress, 0.0f, clamp_max);;
		break;
	}

	_ui_damage->GetComponent<ComponentImage>()->SetColor(float4(1.0f, 1.0f, 1.0f, transparency));

	if (transparency >= clamp_max)
	{
		if (_combat_stats->_current_hp > 1)
		{
			_ui_damage->SetActive(false);
		}

		return;
	}

	_lh_vfx_current_time += Time::DeltaTimeScaled();
}

void  Hachiko::Scripting::PlayerController::IncreaseHealth()
{
	_level_manager->increased_health = true;
	hp_cells.push_back(_hp_cell_extra);
	_combat_stats->_max_hp = 5;
	_combat_stats->Heal(_combat_stats->_max_hp);
	UpdateHealthBar();
}

void Hachiko::Scripting::PlayerController::UpdateEmissives()
{
	if (_heal_fade_progress >= 0.0f) {
		_heal_fade_progress -= Time::DeltaTime() / _heal_effect_fade_duration;
		float progress = _heal_fade_progress / _heal_effect_fade_duration;
		_player_geometry->ChangeEmissiveColor(float4(0.0f, 1.0f, 0.0f, progress), true);

	}
	else if (damage_effect_progress >= 0.0f)
	{
		damage_effect_progress -= Time::DeltaTime() / damage_effect_duration;
		float progress = damage_effect_progress / damage_effect_duration;
		_player_geometry->ChangeEmissiveColor(float4(1.0f, 1.0f, 1.0f, progress), true);

	}
	else
	{
		_player_geometry->ResetEmissive(true);
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
		attack.hit_delay = 0.128f;
		attack.duration = 0.367f; // 10 frames .45ms
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.3f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::RIGHT;
		break;

	case AttackType::COMMON_2:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f; // 9 frames .45ms 0.016
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.3f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::LEFT;
		break;

	case AttackType::COMMON_3:
		attack.hit_delay = 0.266f;
		attack.duration = 0.367f; // 12 frames
		attack.cooldown = 0.4f;
		attack.dash_distance = 1.3f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 2.f;
		attack.stats.range = 4.0f;
		attack.stats.attack_trail = CombatManager::AttackTrail::CENTER;
		break;

		// QUICK ATTACKS
	case AttackType::QUICK_1:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.7f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::RIGHT;
		break;

	case AttackType::QUICK_2:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.7f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::LEFT;
		break;
	case AttackType::QUICK_3:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.1f;
		attack.dash_distance = 0.7f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 1;
		attack.stats.knockback_distance = 0.f;
		// If its cone use degrees on width
		attack.stats.width = 3.f;
		attack.stats.range = 3.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::RIGHT;
		break;

		// HEAVY ATTACKS
	case AttackType::HEAVY_1:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 5.f;
		attack.stats.range = 4.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::RIGHT;
		break;

	case AttackType::HEAVY_2:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 5.f;
		attack.stats.range = 4.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::LEFT;
		break;

	case AttackType::HEAVY_3:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.4f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 2.5f;
		// If its cone use degrees on width
		attack.stats.width = 5.f;
		attack.stats.range = 4.5f;
		attack.stats.attack_trail = CombatManager::AttackTrail::RIGHT;
		break;

		// HAMMER ATTACKS
	case AttackType::HAMMER_1:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.0f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 6.f;
		attack.stats.range = 5.f;
		attack.stats.attack_trail = CombatManager::AttackTrail::RIGHT;
		break;

	case AttackType::HAMMER_2:
		attack.hit_delay = 0.133f;
		attack.duration = 0.367f;
		attack.cooldown = 0.5f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 1.f;
		// If its cone use degrees on width
		attack.stats.width = 6.f;
		attack.stats.range = 5.f;
		attack.stats.attack_trail = CombatManager::AttackTrail::LEFT;
		break;

	case AttackType::HAMMER_3:
		attack.hit_delay = 0.128f;
		attack.duration = 0.367f;
		attack.cooldown = 0.5f;
		attack.dash_distance = 0.5f;
		attack.stats.type = CombatManager::AttackType::RECTANGLE;
		attack.stats.damage = 2;
		attack.stats.knockback_distance = 2.5f;
		// If its cone use degrees on width
		attack.stats.width = 6.f;
		attack.stats.range = 5.f;
		break;

	}
	return attack;
}
