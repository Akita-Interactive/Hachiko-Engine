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
}

void Hachiko::Scripting::DoorController::OnUpdate()
{
	if (Input::IsKeyDown(Input::KeyCode::KEY_1))
	{
		_state = _state == State::CLOSED ? State::OPEN : State::CLOSED;
		UpdateDoorState();
	}
}

void Hachiko::Scripting::DoorController::Open()
{
	_state = State::OPEN;
	UpdateDoorState();
}

void Hachiko::Scripting::DoorController::Close()
{
	_state = State::CLOSED;
	UpdateDoorState();
}

void Hachiko::Scripting::DoorController::UpdateDoorState()
{
	switch (_state)
	{
	case State::CLOSED:
		_door_prefab->SetActive(true);
		_open_door_mesh->SetActive(false);
		door_obstacle->AddObstacle();
		break;
	case State::OPEN:
		door_obstacle->RemoveObstacle();
		_door_prefab->SetActive(false);
		// Small hack to keep things simple, even if the parent is inactive it will still render
		_open_door_mesh->SetActive(true);
		break;
	}
}