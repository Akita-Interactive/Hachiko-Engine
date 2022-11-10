#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "misc/CameraPosChange.h"
#include "entities/player/PlayerCamera.h"

#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::CameraPosChange::CameraPosChange(GameObject* game_object)
	: Script(game_object, "CameraPosChange")
	, _objective(nullptr)
	, _do_look_ahead(false)
	, _do_mouse_movement(true)
    , _speed(0.0f)
    , _duration(0.0f)
    , _relative_position(float3::zero)
    , _rotation(float3::zero)
	, _check_box(false)
{
}

void Hachiko::Scripting::CameraPosChange::OnAwake()
{
	_player = Scenes::GetPlayer();
	_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
	global_pos = game_object->GetTransform()->GetGlobalPosition();
	local_matrix = game_object->GetTransform()->GetLocalMatrix();
	// Adjust bounding box size to object scale
	float3 scale = game_object->GetTransform()->GetLocalScale();
	boundingbox = OBB(global_pos, float3(.5f * scale.x, .5f * scale.y, .5f * scale.z),
		local_matrix.WorldX().Normalized(), local_matrix.WorldY().Normalized(), local_matrix.WorldZ().Normalized());
}

void Hachiko::Scripting::CameraPosChange::OnStart()
{
	_is_inside = false;
}

void Hachiko::Scripting::CameraPosChange::OnUpdate()
{
	if (_player && _camera)
	{
		if (InsideOBB())
		{
			if (_is_inside)
			{
				return;
			}
			_is_inside = true;

			_camera->ChangeRelativePosition(_relative_position, _do_look_ahead, _speed, _duration, _do_mouse_movement);
			_camera->RotateCameraTo(_rotation, _speed);
			if (_objective)
			{
				_camera->SetObjective(_objective);
			}
			else
			{
				// If no objective is set we return to the player
				_camera->SetObjective(_player);
			}
		}
		else
		{
			_is_inside = false;
		}
	}

	if (_check_box)	// Only for debug purposes
	{
		float3 scale = game_object->GetTransform()->GetLocalScale();
		boundingbox = OBB(global_pos, float3(.5f * scale.x, .5f * scale.y, .5f * scale.z),
			local_matrix.WorldX().Normalized(), local_matrix.WorldY().Normalized(), local_matrix.WorldZ().Normalized());
		Debug::DebugDraw(boundingbox, float3(1.0f, 1.0f, 0.0f));
	}
}

bool Hachiko::Scripting::CameraPosChange::InsideOBB()
{
	float3 player_pos = _player->GetTransform()->GetGlobalPosition();
	Sphere hitbox = Sphere(player_pos, 0.5f);

	return boundingbox.Intersects(hitbox);
}