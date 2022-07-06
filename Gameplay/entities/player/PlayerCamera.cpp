#include "scriptingUtil/gameplaypch.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"

// TODO: Delete this include:
#include <core/Scene.h>
#include <thread>
#include "constants/Sounds.h"

Hachiko::Scripting::PlayerCamera::PlayerCamera(GameObject* game_object)
	: Script(game_object, "PlayerCamera")
	, _relative_position_to_player(math::float3(0.0f, 19.0f, 13.0f))
	, _objective(nullptr)
	, _follow_delay(0.6f)
{
}

void Hachiko::Scripting::PlayerCamera::OnAwake()
{
	if (_objective != nullptr)
	{
		_current_objective = _objective;
		_player_ctrl = _objective->GetComponent<PlayerController>();
	}
	_look_ahead = float3::zero;
	_relative_position_aux = _relative_position_to_player;
	_updated_relative_position = _relative_position_to_player;
	_updated_rotation = game_object->GetTransform()->GetGlobalRotation();
	// Seed the rand() for the shaking
	srand(static_cast <unsigned> (time(0)));
}

void Hachiko::Scripting::PlayerCamera::OnStart()
{
}

void Hachiko::Scripting::PlayerCamera::OnUpdate()
{
	CheckForObjective();

	RecalculateRelativePos();

	RecalculateRotation();

	SetLookAhead();

	MoveCamera();
}

/// <summary>
/// Checks if the objective has changed
/// </summary>
void Hachiko::Scripting::PlayerCamera::CheckForObjective()
{
	if (_current_objective != _objective)
	{
		_current_objective = _objective;
		if (_current_objective != nullptr)
		{
			_player_ctrl = _current_objective->GetComponent<PlayerController>();
		}
	}
}

void Hachiko::Scripting::PlayerCamera::SetLookAhead()
{
	if (_player_ctrl && _player_ctrl->_state == PlayerState::WALKING)
	{
		const float look_ahead_time = Time::DeltaTime() / 0.8f;
		Clamp<float>(look_ahead_time, 0.0f, 1.0f);
		_look_ahead = math::float3::Lerp(_look_ahead, _current_objective->GetTransform()->GetFront() * 5, look_ahead_time);
	}
	else
	{
		_look_ahead = float3::zero;
	}
}

void Hachiko::Scripting::PlayerCamera::MoveCamera()
{
	const math::float2 mouse_movement_x_z = MoveCameraWithMouse();
	const math::float3 mouse_movement = float3(mouse_movement_x_z.x, 0.0f, mouse_movement_x_z.y);
	ScrollWheelZoom(&_relative_position_to_player);

	const math::float3 final_position = _current_objective->GetTransform()->GetGlobalPosition()
	 + _look_ahead + mouse_movement + _relative_position_to_player;
	ComponentTransform* transform = game_object->GetTransform();
	math::float3 current_position = transform->GetGlobalPosition();

	float delayed_time = _is_in_position ? Time::DeltaTime() / _follow_delay : _reposition_progress;
	Clamp<float>(delayed_time, 0.0f, 1.0f);

	// Lerp to the position to the player with a delay: 
	current_position = math::float3::Lerp(current_position, final_position,
		delayed_time);

	float3 shake_offset = float3::zero;

	shake_offset = Shake();

	transform->SetGlobalPosition(current_position + shake_offset);
}

float2 Hachiko::Scripting::PlayerCamera::MoveCameraWithMouse()
{
	float2 added_movement, mouse_pos;
	mouse_pos = Input::GetMouseNormalizedPosition();
	mouse_pos = mouse_pos - float2(0.5f, 0.5f);
	if (mouse_pos.x > -0.25f && mouse_pos.x < 0.25f)
	{
		mouse_pos.x = 0;
	}
	else
	{
		mouse_pos.x += (mouse_pos.x > 0) ? -0.25f : +0.25f;
	}

	if (mouse_pos.y > -0.25f && mouse_pos.y < 0.25f)
	{
		mouse_pos.y = 0;
	}
	else
	{
		mouse_pos.y += (mouse_pos.y > 0) ? -0.25f : +0.25f;
	}
	const float intensity = 3.0f;
	// We multiply by 4 to set it to 0-1 scale
	added_movement = (mouse_pos * 4.0f) * intensity;
	added_movement = added_movement.Clamp(float2(-intensity, -intensity), float2(intensity, intensity));
	return added_movement;
}

void Hachiko::Scripting::PlayerCamera::ScrollWheelZoom(float3* cam_pos)
{
	int scroll_delta = Input::GetScrollWheelDelta();

	if (scroll_delta > 0 && cam_pos->y > 6.0f)
	{
		cam_pos->y = cam_pos->y - 0.5f;
		cam_pos->z = cam_pos->z - 0.3f;
	}
	else if (scroll_delta < 0 && cam_pos->y < 30.0f)
	{
		cam_pos->y = cam_pos->y + 0.5f;
		cam_pos->z = cam_pos->z + 0.3f;
	}
}

void Hachiko::Scripting::PlayerCamera::Shake(float time, float intensity)
{
	shake_time = time;
	shake_elapsed = 0.0f;
	shake_intensity = intensity;
	shake_magnitude = 1.0f;
}

float3 Hachiko::Scripting::PlayerCamera::Shake()
{
	if (shake_elapsed < shake_time)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float x = (r - 0.5f) * shake_magnitude * shake_intensity;
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float z = (r - 0.5f) * shake_magnitude * shake_intensity;

		shake_elapsed += Time::DeltaTime();
		shake_magnitude = (1 - (shake_elapsed / shake_time)) * (1 - (shake_elapsed / shake_time));
		return float3(x, 0, z);
	} 
	else
	{
		shake_time = 0.0f;
		shake_elapsed = 0.0f;
		return float3::zero;
	}
}

void Hachiko::Scripting::PlayerCamera::ChangeRelativePosition(math::float3 new_relative_position, float speed, float time)
{
	if (time > 0.0f)
	{
		// By reverting position we prevent overlaping
		RevertRelativePosition();
		_is_temporary_moved = true;
		_position_timer = time;
	}
	_relative_position_aux = _relative_position_to_player;
	_updated_relative_position = new_relative_position;
	_is_in_position = false;
	if (speed > 0.0f)
	{
		_reposition_time = 1 / speed;
		_reposition_progress = 0.0f;
	}
	else
	{
		_reposition_time = 1.0f;
		_reposition_progress = 1.0f;
	}
}

void Hachiko::Scripting::PlayerCamera::RevertRelativePosition(float speed)
{
	_updated_relative_position = _relative_position_aux;
	_is_in_position = false;
	if (speed > 0.0f)
	{
		_reposition_time = 1.0f / speed;
		_reposition_progress = 0.0f;
	}
	else
	{
		_relative_position_to_player = _relative_position_aux;
		_reposition_time = 1.0f;
		_reposition_progress = 1.0f;
	}
}

void Hachiko::Scripting::PlayerCamera::SwitchRelativePosition(math::float3 new_relative_position, float speed)
{
	if (new_relative_position.Equals(_updated_relative_position))
	{
		RevertRelativePosition(speed);
	}
	else
	{
		ChangeRelativePosition(new_relative_position, speed);
	}
}

void Hachiko::Scripting::PlayerCamera::RecalculateRelativePos()
{
	if (!_is_in_position)
	{
		if (_reposition_progress > 1.0f)
		{
			_is_in_position = true;
		}
		_relative_position_to_player = math::Lerp(_relative_position_to_player, _updated_relative_position, _reposition_progress);
		_reposition_progress += Time::DeltaTime() / _reposition_time;
	}
	else
	{
		if (_is_temporary_moved)
		{
			if (_position_timer <= 0.0f)
			{
				RevertRelativePosition();
				_is_temporary_moved = false;
			}
			else
			{
				_position_timer -= Time::DeltaTime();
			}
		}
	}
}

void Hachiko::Scripting::PlayerCamera::RotateCameraTo(math::float3 new_rotation, float speed)
{
	_is_in_rotation = false;
	float3 rotation = DegToRad(new_rotation);
	_updated_rotation = math::Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	if (speed > 0.0f)
	{
		_rotation_time = 1.0f / speed;
		_rotation_progress = 0.0f;
	}
	else
	{
		_rotation_time = 1.0f;
		_rotation_progress = 1.0f;
	}
}

void Hachiko::Scripting::PlayerCamera::RecalculateRotation()
{
	if (!_is_in_rotation)
	{
		if (_rotation_progress > 1.0f)
		{
			_is_in_rotation = true;
		}
		game_object->GetTransform()->SetLocalRotation(math::Slerp(game_object->GetTransform()->GetLocalRotation(), _updated_rotation, _rotation_progress));
		_rotation_progress += Time::DeltaTime() / _rotation_time;
	}
}