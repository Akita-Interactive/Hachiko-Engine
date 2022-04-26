#include "scriptingUtil/gameplaypch.h"
#include "PlayerCamera.h"
#include <components/ComponentTransform.h>

Hachiko::Scripting::PlayerCamera::PlayerCamera(GameObject* game_object)
	: Script(game_object, "PlayerCamera")
	, _distance_to_player(math::float3::zero)
	, _player(nullptr)
{
}

void Hachiko::Scripting::PlayerCamera::OnAwake()
{
	_distance_to_player = math::float3(0.0f, 50.0f, 40.0f);

	std::string t_name;
	int i = 0;
	while (t_name != "Player")
	{
		t_name = game_object->parent->children[i]->name;
		i++;
	}
	_player = game_object->parent->children[i - 1];

	HE_LOG("PLAYER: %s", _player->name.c_str());
}

void Hachiko::Scripting::PlayerCamera::OnStart()
{
}

void Hachiko::Scripting::PlayerCamera::OnUpdate()
{
	ComponentTransform* transform = game_object->GetTransform();
	transform->LookAtTarget(_player->GetTransform()->GetGlobalPosition());
	transform->SetGlobalPosition(
		_player->GetTransform()->GetGlobalPosition() + _distance_to_player);
}