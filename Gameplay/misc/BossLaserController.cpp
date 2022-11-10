#include "scriptingUtil/gameplaypch.h"
#include "BossLaserController.h"
#include "constants/Scenes.h"
#include "constants/Sounds.h"
#include "entities/player/PlayerController.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::BossLaserController::BossLaserController(GameObject* game_object)
	: Script(game_object, "BossLaserController")
	, _laser(nullptr)
	, _max_length(5.0f)
	, _max_scale(0.2f)
	, _activation_time(1.0f)
	, _damage(1.0f)
	, _track_if_active(false)
	, _tracking_speed(20.0f)
	, _toggle_activation(false)
	, _toggle_active_time(1.0f)
	, _toggle_inactive_time(1.0f)
	, _audio_source(nullptr)
{
}

void Hachiko::Scripting::BossLaserController::OnAwake()
{
	_terrain = Scenes::GetTerrainContainer();
	_player = Scenes::GetPlayer();

	_audio_source = game_object->GetComponent<ComponentAudioSource>();
	_audio_source->PostEvent(Hachiko::Sounds::PLAY_LASER);
}

void Hachiko::Scripting::BossLaserController::OnUpdate()
{
	RotateTowardsPlayer();

	switch (_state)
	{
	case ACTIVE:
		AdjustLength();
		CheckPlayerCollision();
		if (_toggle_activation)
		{
			_elapsed_time += Time::DeltaTimeScaled();
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
		_elapsed_time += Time::DeltaTimeScaled();
		_scale = (_elapsed_time / _activation_time) * _max_scale;
		if (_elapsed_time >= _activation_time)
		{
			ChangeState(ACTIVE);
		}
		break;

	case INACTIVE:
		if (_toggle_activation)
		{
			_elapsed_time += Time::DeltaTimeScaled();
			if (_elapsed_time >= _toggle_inactive_time)
			{
				ChangeState(ACTIVATING);
			}
		}
		break;
	}
}

void Hachiko::Scripting::BossLaserController::ChangeState(State new_state)
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
		_laser->ChangeEmissiveColor(float4(1.0f, 1.0f, 1.0f, 0.2f), true);
		break;

	case State::INACTIVE:
		_elapsed_time = 0.0f;
		_laser->SetActive(false);
		break;
	}
	_state = new_state;
}

void Hachiko::Scripting::BossLaserController::RotateTowardsPlayer()
{
	if (_track_if_active || _state != State::ACTIVE)
	{
		ComponentTransform* transform = game_object->GetTransform();
		float3 euler_rotation = game_object->GetTransform()->GetLocalRotationEuler();
		// We get the direction vector towards the player
		float3 direction = (_player->GetTransform()->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		// The front vector of the laser and we get the 90 degrees clockwise vector
		const float3 front = transform->GetFront().Normalized();
		const float3 base_rotated = float3(-front.z, 0, front.x);
		// We set direction.y to 0 for more simple calculus
		direction.y = 0.0f;
		// We check the rotation direction with the dot product
		float rotation_direction = base_rotated.Dot(direction);

		if (rotation_direction > 0.0f)
		{
			rotation_direction = 1.0f;
		}
		else if (rotation_direction < 0.0f)
		{
			rotation_direction = -1.0f;
		}

		euler_rotation.y += Time::DeltaTimeScaled() * _tracking_speed * rotation_direction;
		transform->SetLocalRotationEuler(euler_rotation);
	}
}

void Hachiko::Scripting::BossLaserController::AdjustLength()
{
	float3 collision_point;
	float new_length = _max_length;

	const float3 start = _laser->GetTransform()->GetGlobalPosition();
	const float3 end = start + _laser->GetTransform()->GetFront() * _max_length;

	const GameObject* terrain_hit = SceneManagement::RayCast(start, end, &collision_point, _terrain);
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

void Hachiko::Scripting::BossLaserController::CheckPlayerCollision() const
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
		if (_player->GetComponent<PlayerController>()->RegisterHit(_damage, true, knockback, false, PlayerController::DamageType::LASER))
		{
			_audio_source->PostEvent(Hachiko::Sounds::PLAY_LASER_HIT);
		}
	}
}
