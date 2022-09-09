#include "scriptingUtil/gameplaypch.h"
#include "misc/FancyLights.h"

Hachiko::Scripting::FancyLights::FancyLights(Hachiko::GameObject* game_object)
	: Script(game_object, "FancyLights")
	, _radius(0.0f)
	, _distance(0.0f)
	, _speed(0.0f)
	, _rotate_on_y(false)
    , _angle(0.0f)
    , _point_light(nullptr)
{
}

void Hachiko::Scripting::FancyLights::OnAwake()
{
	_point_light = game_object->GetComponent<ComponentPointLight>();

	math::LCG lcg(game_object->GetID());

	_distance = 2.0f;
	_speed = 5.0f;
	_radius = 2.0f;

	_rotate_on_y = game_object->GetName() == "Light 1";

	_point_light->color = float4::RandomGeneral(lcg, 0.0f, 1.0f);
}

void Hachiko::Scripting::FancyLights::OnUpdate()
{
	float velocity = Time::DeltaTime() * _speed;
	_angle += velocity;

	math::float3 center = game_object->parent->GetTransform()->GetGlobalPosition();

	if (_rotate_on_y)
	{
		game_object->GetTransform()->SetGlobalPosition(math::float3(std::sin(_angle) * _distance + center.x, std::cos(_angle) * _distance + center.y, center.z));
	}
	else
	{
		game_object->GetTransform()->SetGlobalPosition(math::float3(std::sin(_angle) * _distance + center.x, center.y, std::cos(_angle) * _distance + center.z));
	}
}