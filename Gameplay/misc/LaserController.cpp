#include "scriptingUtil/gameplaypch.h"
#include "LaserController.h"
#include "constants/Scenes.h"
#include "constants/Sounds.h"
#include "entities/player/PlayerController.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::LaserController::LaserController(GameObject* game_object)
	: Script(game_object, "LaserController")
	, _laser(nullptr)
	, _max_length(5.0f)
	, _max_scale(0.2f)
	, _activation_time(1.0f)
	, _damage(1.0f)
	, _spin_movement(false)
	, _spin_speed(20.0f)
	, _horizonal_movement(false)
	, _movement_speed(1.0f)
	, _movement_target(nullptr)
	, _toggle_activation(false)
	, _toggle_active_time(1.0f)
	, _toggle_inactive_time(1.0f)
	, _audio_source(nullptr)
{
}

void Hachiko::Scripting::LaserController::OnAwake()
{
	_terrain = Scenes::GetTerrainContainer();
	_player = Scenes::GetPlayer();

	_initial_position = game_object->GetTransform()->GetGlobalPosition();
	_audio_source = game_object->GetComponent<ComponentAudioSource>();
	_audio_source->PostEvent(Hachiko::Sounds::PLAY_LASER);
}

void Hachiko::Scripting::LaserController::OnUpdate()
{
	if (_spin_movement)
	{
		float3 euler_rotation = game_object->GetTransform()->GetLocalRotationEuler();
		euler_rotation.y += Time::DeltaTime() * _spin_speed;
		game_object->GetTransform()->SetLocalRotationEuler(euler_rotation);
	}

	if (_horizonal_movement && _movement_target != nullptr)
	{
		_movement_position += (_movement_forward_direction)
			? _movement_speed * Time::DeltaTime()
			: _movement_speed * Time::DeltaTime() * -1.0f;

		if (_movement_position > 1.0f)
		{
			_movement_forward_direction = false;
			_movement_position += (1.0f - _movement_position);
		}
		else if (_movement_position < 0.0f)
		{
			_movement_forward_direction = true;
			_movement_position *= -1.0f;
		}

		game_object->GetTransform()->SetGlobalPosition(
			float3::Lerp(_initial_position, _movement_target->GetTransform()->GetGlobalPosition(),
				_movement_position * _movement_position * (3 - 2 * _movement_position))
		);
	}

	switch (_state)
	{
	case ACTIVE:
		AdjustLength();
		CheckPlayerCollision();
		if (_toggle_activation)
		{
			_elapsed_time += Time::DeltaTime();
			if (_elapsed_time >= _toggle_active_time)
			{
				ChangeState(INACTIVE);
			}
		}
		break;

	case ACTIVATING:
		if (!_laser->IsActive())
		{
			_laser->SetActive(true);
		}

		AdjustLength();
		_elapsed_time += Time::DeltaTime();
		_scale = (_elapsed_time / _activation_time) * _max_scale;
		if (_elapsed_time >= _activation_time)
		{
			ChangeState(ACTIVE);
		}
		break;

	case INACTIVE:
		if (_toggle_activation)
		{
			_elapsed_time += Time::DeltaTime();
			if (_elapsed_time >= _toggle_inactive_time)
			{
				ChangeState(ACTIVATING);
			}
		}
		break;
	}
}

void Hachiko::Scripting::LaserController::ChangeState(State new_state)
{
	switch (new_state)
	{
	case ACTIVE:
		_elapsed_time = 0.0f;
		_scale = _max_scale;
		break;

	case State::ACTIVATING:
		_audio_source->PostEvent(Hachiko::Sounds::PLAY_LASER);
		_elapsed_time = 0.0f;
		_scale = 0.0f;
		AdjustLength();
		_laser->ChangeEmissiveColor(float4(1.0f, 1.0f, 1.0f, 0.2f), _activation_time, true);
		break;

	case State::INACTIVE:
		_audio_source->PostEvent(Hachiko::Sounds::STOP_LASER);
		_elapsed_time = 0.0f;
		_laser->SetActive(false);
		break;
	}
	_state = new_state;
}

void Hachiko::Scripting::LaserController::AdjustLength()
{
	float3 collision_point;
	float new_length = _max_length;

	const float3 start = _laser->GetTransform()->GetGlobalPosition();
	const float3 end = start + _laser->GetTransform()->GetFront() * _max_length;

	const GameObject* terrain_hit = SceneManagement::Raycast(start, end, &collision_point, _terrain);
	if (terrain_hit != nullptr)
	{
		new_length = std::min(start.Distance(collision_point), _max_length);
	}

	if (_length != new_length || _state == ACTIVATING)
	{
		_length = new_length;
		_laser->GetTransform()->SetLocalScale(float3(_scale, _scale, _length * 0.5f));
	}
}

void Hachiko::Scripting::LaserController::CheckPlayerCollision() const
{
	float3 intersection_point;
	const float3 player_pos = _player->GetComponent<ComponentTransform>()->GetGlobalPosition();
	const OBB* laser_obb = _laser->GetFirstMeshRendererOBB();

	const Capsule hitbox = Capsule(player_pos, player_pos + float3(0, 3, 0), 0.5);

	if (laser_obb->Intersects(hitbox))
	{
		vec intersection_point = laser_obb->ClosestPoint(player_pos);

		ComponentTransform* transform = _laser->GetComponent<ComponentTransform>();
		const float3 point_in_line = transform->GetGlobalPosition();
		const float3 line = transform->GetFront();
		const float torque = line.x * (intersection_point.z - point_in_line.z) - line.z * (intersection_point.x -
			point_in_line.x);

		float3 knockback = transform->GetRight();
		if (torque < 0)
		{
			knockback *= -1;
		}

		// sound only if hit
		if (_player->GetComponent<PlayerController>()->RegisterHit(_damage, true, knockback))
		{
			_audio_source->PostEvent(Hachiko::Sounds::PLAY_LASER_HIT);
		}
	}
}
