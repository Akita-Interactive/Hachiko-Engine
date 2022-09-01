#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "misc/CameraPosChange.h"
#include "entities/player/PlayerCamera.h"

#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::CameraPosChange::CameraPosChange(GameObject* game_object)
	: Script(game_object, "CameraPosChange")
	, _objective(nullptr)
	, _do_look_ahead(false)
    , _speed(0.0f)
    , _duration(0.0f)
    , _relative_position(float3::zero)
    , _rotation(float3::zero)
{
}

void Hachiko::Scripting::CameraPosChange::OnAwake()
{
	_player = Scenes::GetPlayer();
	_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
}

void Hachiko::Scripting::CameraPosChange::OnStart()
{
}

void Hachiko::Scripting::CameraPosChange::OnUpdate()
{
	if (_player && _camera)
	{
		float distance = Distance(game_object->GetTransform()->GetGlobalPosition(), _player->GetTransform()->GetGlobalPosition());
		if (distance < 3.0f)
		{
			_is_inside = true;
		}
		else 
		{
			if (_is_inside)
			{
				_camera->ChangeRelativePosition(_relative_position, _do_look_ahead, _speed, _duration);
				_camera->RotateCameraTo(_rotation, _speed);
				if (_objective)
				{
					_camera->SetObjective(_objective);
				}
			}
			_is_inside = false;
		}
	}
}
