#include "scriptingUtil/gameplaypch.h"
#include "PlayerCamera.h"
#include <components/ComponentTransform.h>

Hachiko::Scripting::PlayerCamera::PlayerCamera(GameObject* game_object)
	: Script(game_object, "PlayerCamera")
	, _relative_position_to_player(math::float3::zero)
	, _player(nullptr)
	, _follow_delay(0.0f)
{
}

void Hachiko::Scripting::PlayerCamera::OnAwake()
{
	_relative_position_to_player = math::float3(0.0f, 15.0f, 10.0f);

	// This is until we have saving for scripts. Because of this, this
	// script needs to be a direct child of scene root, and there must
	// be a GameObject of name "PlayerC" which is our player.
	_player = game_object->parent->GetFirstChildWithName("PlayerC");

	_follow_delay = 0.8f;
}

void Hachiko::Scripting::PlayerCamera::OnStart()
{
}

void Hachiko::Scripting::PlayerCamera::OnUpdate()
{
	float2 mouse_movement = MoveCameraWithMouse();
	const math::float3 final_position = _player->GetTransform()->GetGlobalPosition()
		+ _relative_position_to_player + float3(mouse_movement.x, 0.0f, mouse_movement.y);
	ComponentTransform* transform = game_object->GetTransform();
	math::float3 current_position = transform->GetGlobalPosition();

	ScrollWheelZoom(&_relative_position_to_player);

	if (!current_position.Equals(final_position))
	{
		const float delayed_time = Time::DeltaTime() / _follow_delay;
		Clamp<float>(delayed_time, 0.0f, 1.0f);

		// Comment the following if you want z follow to be delayed as well:
		//current_position.z = final_position.z;

		// Lerp to the pre-defined relative position to the player with a delay: 
		current_position = math::float3::Lerp(current_position, final_position,
			delayed_time);


		math::float3 relative_position = final_position - current_position;

		constexpr float x_clamp = 2.25f;
		constexpr float z_clamp = 2.5f;

		if (relative_position.z > z_clamp - mouse_movement.y)
		{
			current_position.z = final_position.z - z_clamp + mouse_movement.y;
		}
		if (relative_position.z < -z_clamp - mouse_movement.y)
		{
			current_position.z = final_position.z + z_clamp + mouse_movement.y;
		}

		if (relative_position.x > x_clamp - mouse_movement.x)
		{
			current_position.x = final_position.x - x_clamp + mouse_movement.x;
		}
		if (relative_position.x < -x_clamp - mouse_movement.x)
		{
			current_position.x = final_position.x + x_clamp + mouse_movement.x;
		}

		transform->SetGlobalPosition(current_position);
	}

	// Uncomment the following line if you want the camera to turn itself towards
	// curent player position:
	//transform->LookAtTarget(_player->GetTransform()->GetGlobalPosition());
}

float2 Hachiko::Scripting::PlayerCamera::MoveCameraWithMouse()
{
	float2 added_movement, mouse_pos;
	mouse_pos = Input::GetMousePosition();
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

	added_movement = mouse_pos * 6;
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