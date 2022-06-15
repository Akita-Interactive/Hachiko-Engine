#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/BulletController.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

#define RAD_TO_DEG 180.0f / math::pi

Hachiko::Scripting::PlayerController::PlayerController(GameObject* game_object)
	: Script(game_object, "PlayerController")
	, _attack_indicator(nullptr)
	, _goal(nullptr)
	, _dash_duration(0.0f)
	, _dash_distance(0.0f)
	, _dash_cooldown(0.0f)
	, _attack_duration(0.0f)
	, _rotation_duration(0.0f)
	, _combat_stats()
	, _max_dash_charges(0)
	, _state(PlayerState::IDLE)
	, _camera(nullptr)
	, _ui_damage(nullptr)
{
}

void Hachiko::Scripting::PlayerController::OnAwake()
{
	_dash_charges = _max_dash_charges;

	if (_attack_indicator)
	{
		_attack_indicator->SetActive(false);
	}
	if (_ui_damage)
	{
		_ui_damage->SetActive(false);
	}
	enemies = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");
	dynamic_envi = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");

	_combat_stats = game_object->GetComponent<Stats>();
	_combat_stats->_attack_power = 2;
	_combat_stats->_attack_cd = 1;
	_combat_stats->_move_speed = 7.0f;
	_combat_stats->_max_hp = 4;
	_combat_stats->_current_hp = _combat_stats->_max_hp;

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

void Hachiko::Scripting::PlayerController::OnUpdate()
{
	_player_transform = game_object->GetTransform();
	_player_position = _player_transform->GetGlobalPosition();
	_movement_direction = float3::zero;

	if (_combat_stats->_current_hp <= 0)
	{
		//SceneManagement::SwitchScene(Scenes::LOSE);
		HE_LOG("YOU DIED");
	}

	// Handle player the input
	HandleInputAndStatus();

	// Run movement simulation
	MovementController();

	// Run attack simulation
	AttackController();

	// Rotate player to the necessary direction:
	WalkingOrientationController();

	// Apply the position after everything is simulated
	_player_transform->SetGlobalPosition(_player_position);
}

PlayerState Hachiko::Scripting::PlayerController::GetState() const
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
		if (Input::IsMouseButtonDown(Input::MouseButton::RIGHT))
		{
			RangedAttack();
		}
		else if (Input::IsMouseButtonDown(Input::MouseButton::LEFT))
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

		_state = PlayerState::IDLE;
		if (!_god_mode)
		{
			float3 corrected_position = Navigation::GetCorrectedPosition(_player_position, float3(3.0f, 3.0f, 3.0f));
			if (corrected_position.x < FLT_MAX)
			{
				_player_position = corrected_position;
			}
			// Started falling
			_state = PlayerState::FALLING;
		}
	}
}

void Hachiko::Scripting::PlayerController::Dash()
{
	_state = PlayerState::DASHING;
	_dash_charges -= 1;
	_dash_progress = 0.f;
	_dash_start = _player_position;

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
	float3 dash_final_position = _dash_start + _dash_direction * _dash_distance;
	corrected_dash_final_position = Navigation::GetCorrectedPosition(corrected_dash_final_position, float3(0.5f, 0.1f, 0.5f));
	if (corrected_dash_final_position.x < FLT_MAX)
	{
		_dash_end = corrected_dash_final_position;
	}
	else
	{
		_dash_end = dash_final_position;
	}
}


void Hachiko::Scripting::PlayerController::MeleeAttack()
{
	_state = PlayerState::MELEE_ATTACKING;
	_attack_current_duration = _attack_duration; // For now we'll focus on melee attacks
	_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
	if (enemies == nullptr && dynamic_envi == nullptr) {
		return;
	}

	std::vector<GameObject*> enemy_children = enemies ? enemies->children : std::vector<GameObject*>();
	std::vector<GameObject*> environment = dynamic_envi ? dynamic_envi->children : std::vector<GameObject*>();

	// MELEE

	enemy_children.insert(enemy_children.end(), environment.begin(), environment.end());

	GameObject* inter = game_object->scene_owner->GetRoot();

	std::vector<GameObject*> elements_hit = {};
	//EnemyControler* enemy_ctrl = _player->GetComponent<PlayerController>();
	math::float4x4 inv_matrix = _player_transform->GetGlobalMatrix().Transposed();
	for (int i = 0; i < enemy_children.size(); ++i)
	{
		if (enemy_children[i]->active && _combat_stats->_attack_range >= _player_transform->GetGlobalPosition().Distance(enemy_children[i]->GetTransform()->GetGlobalPosition()))
		{
			float3 normalized_center = _player_transform->GetFront().Normalized();
			float3 normalized_enemy = (enemy_children[i]->GetTransform()->GetGlobalPosition() - _player_transform->GetGlobalPosition()).Normalized();
			float dot_product = normalized_center.Dot(normalized_enemy);
			float angle_of_enemy = std::acos(dot_product) * RAD_TO_DEG;
			float attack_angle = 60.0f; // This can vary in the future deppending of weapon
			if (angle_of_enemy < attack_angle)
			{
				elements_hit.push_back(enemy_children[i]);
			}
		}
	}

	//loop in enemies hit
	for (Hachiko::GameObject* element : elements_hit)
	{

		EnemyController* enemy_controller = element->GetComponent<EnemyController>();
		CrystalExplosion* crystal_explotion = element->GetComponent<CrystalExplosion>();

		float3 relative_dir = element->GetTransform()->GetGlobalPosition() - _player_transform->GetGlobalPosition();

		if (enemy_controller != nullptr)
		{
			enemy_controller->RegisterHit(_combat_stats->_attack_power, relative_dir.Normalized());
		}
		else if (crystal_explotion != nullptr)
		{
			crystal_explotion->RegisterHit(_combat_stats->_attack_power);
		}
	}

	if (elements_hit.size() > 0)
	{
		_camera->GetComponent<PlayerCamera>()->Shake(0.6f, 0.2f);
	}

	// Move player a bit forward on melee attack
	_player_position += _player_transform->GetFront() * 0.3f;
	_player_position = Navigation::GetCorrectedPosition(_player_position, float3(2.0f, 1.0f, 2.0f));
	_attack_current_cd = _combat_stats->_attack_cd;
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


bool Hachiko::Scripting::PlayerController::IsActionLocked() const
{
	return IsDashing() || IsStunned() || IsAttacking() || IsFalling();
}

void Hachiko::Scripting::PlayerController::RangedAttack()
{
	_state = PlayerState::RANGED_ATTACKING;
	_player_transform->LookAtTarget(GetRaycastPosition(_player_position));
	const float3 forward = _player_transform->GetFront().Normalized();
	float3 attack_origin_position = _player_position;
	attack_origin_position.y += 0.5f;

	// Spawn bullet (Passing the prefab can be improved)
	UID bullet_uid = 14999767472668584259;
	GameObject* bullet = GameObject::Instantiate(&bullet_uid, game_object->scene_owner->GetRoot());

	bullet->GetTransform()->SetGlobalPosition(attack_origin_position);
	bullet->GetComponent<BulletController>()->SetForward(forward);
	bullet->GetComponent<BulletController>()->SetDamage(_combat_stats->_attack_power);

	_attack_current_cd = _combat_stats->_attack_cd;
}

void Hachiko::Scripting::PlayerController::MovementController()
{
	DashController();
	WalkingOrientationController();

	if (IsWalking())
	{
		_player_position += (_movement_direction * _combat_stats->_move_speed * Time::DeltaTime());
	}

	if (_god_mode)
	{
		return;
	}

	if (IsFalling())
	{
		_player_position.y -= 0.25f;

		if (_dash_start.y - _player_position.y > _falling_distance)
		{
			_player_position = _dash_start;
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
			_player_position = Navigation::GetCorrectedPosition(_knock_end, float3(2.0f, 2.0f, 2.0f));
			_state = PlayerState::IDLE;
		}
	}

	float3 corrected_position = Navigation::GetCorrectedPosition(_player_position, float3(3.0f, 3.0f, 3.0f));
	if (Distance(corrected_position, _player_position) < 1.0f)
	{
		_player_position = corrected_position;
		if (IsFalling())
		{
			// Stopped falling
			_state = PlayerState::IDLE;
		}
	}
	else if (!IsDashing())
	{
		// Started falling
		_state = PlayerState::FALLING;
	}
}

void Hachiko::Scripting::PlayerController::DashController()
{
	DashChargesManager();

	if (!IsDashing())
	{
		return;
	}

	_dash_progress += Time::DeltaTime() / _dash_duration;
	_dash_progress = _dash_progress > 1.0f ? 1.0f : _dash_progress;

	// TODO: Instead of approaching to _dash_end linearly, dash must have some sort
	// of an acceleration.
	_player_position = math::float3::Lerp(_dash_start, _dash_end,
		_dash_progress);

	if (_dash_progress >= 1.0f)
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
		return;
	}

	if (_attack_current_duration > 0.0f)
	{
		if (_attack_indicator)
		{
			_attack_indicator->SetActive(true);
		}

		_attack_current_duration -= Time::DeltaTime();
	}
	else
	{
		if (_attack_indicator)
		{
			_attack_indicator->SetActive(false);
		}
		// Set state to idle, it will be overriden if there is a movement:
		_state = PlayerState::IDLE;
	}

	if (_attack_current_cd > 0.0f)
	{
		if (_god_mode)
		{
			return;
		}
		_attack_current_cd -= Time::DeltaTime();
	}
}

void Hachiko::Scripting::PlayerController::RegisterHit(float damage_received, bool is_heavy, float3 direction)
{
	if (_god_mode)	return;

	_combat_stats->ReceiveDamage(damage_received);
	UpdateHealthBar();
	game_object->ChangeColor(float4(255, 255, 255, 255), 0.3);

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

	// Player is dead
	if (!_combat_stats->IsAlive())
	{
		SceneManagement::SwitchScene(Scenes::LOSE);
	}
}

void Hachiko::Scripting::PlayerController::RecieveKnockback(math::float3 direction)
{
	_state = PlayerState::STUNNED;
	_knock_start = _player_transform->GetGlobalPosition();
	_knock_end = _player_transform->GetGlobalPosition() + direction;
	_stun_time = _stun_duration;
	_player_transform->LookAtDirection(-direction);
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
	}
}

void Hachiko::Scripting::PlayerController::CheckGoal(const float3& current_position)
{
	const float3 goal_position = _goal->GetTransform()->GetGlobalPosition();

	if (Distance(current_position, goal_position) < 10.0f)
	{
		SceneManagement::SwitchScene(12124061992092393469);
	}
}
