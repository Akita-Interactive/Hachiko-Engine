#include "scriptingUtil/gameplaypch.h"
#include "DoorController.h"
#include "components/ComponentObstacle.h"

Hachiko::Scripting::DoorController::DoorController(GameObject* game_object)
	: Script(game_object, "DoorController")
	, _door_prefab(nullptr)
{}

void Hachiko::Scripting::DoorController::OnAwake()
{
	door_obstacle = _door_prefab->GetComponent<ComponentObstacle>();
	_closed_door_mesh = _door_prefab->children[0];
	_open_door_mesh = _door_prefab->children[1];
	Close();
	_initial_z = _closed_door_mesh->GetTransform()->GetLocalPosition().y;
}

void Hachiko::Scripting::DoorController::OnUpdate()
{
	UpdateDoorState();
}

void Hachiko::Scripting::DoorController::Open()
{
	_state = State::OPENING;
}

void Hachiko::Scripting::DoorController::Close()
{
	_state = State::CLOSED;
}

void Hachiko::Scripting::DoorController::UpdateDoorState()
{
	if (_prev_state == _state && _state != State::OPENING)
	{
		return;
	}
	_prev_state = _state;

	switch (_state)
	{
	case State::CLOSED:
		_door_prefab->SetActive(true);
		_open_door_mesh->SetActive(false);
		door_obstacle->AddObstacle();
		break;
	case State::OPENING:
		_open_door_mesh->SetActive(true);
		_closed_door_mesh->GetTransform()->SetLocalPosition(_closed_door_mesh->GetTransform()->GetLocalPosition() - float3(0.0f, 0.0f, 0.1f));
		if (math::Abs(_initial_z - _closed_door_mesh->GetTransform()->GetLocalPosition().z) >= 1.1f)
		{
			_state = State::OPEN;
		}
		break;
	case State::OPEN:
		door_obstacle->RemoveObstacle();
		_door_prefab->SetActive(false);
		// Small hack to keep things simple, even if the parent is inactive it will still render
		_open_door_mesh->SetActive(true);
		break;
	}
}