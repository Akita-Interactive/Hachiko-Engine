#include "scriptingUtil/gameplaypch.h"
#include "DynamicCamera.h"
#include <components/ComponentTransform.h>

Hachiko::Scripting::DynamicCamera::DynamicCamera(GameObject* game_object)
	: Script(game_object, "DynamicCamera")
	, _start_point(math::float3::zero)
	, _end_point(math::float3::zero)
	, _speed(0.0f)
	, _lerp_position(0.0f)
{
}

void Hachiko::Scripting::DynamicCamera::OnAwake()
{
	_start_point = game_object->GetTransform()->GetGlobalPosition();
	_end_point = math::float3(-10.0f, 3.0f, -4.0f);
	_speed = 5.0f;
	_lerp_position = 0.0f;
}

void Hachiko::Scripting::DynamicCamera::OnStart()
{
}

void Hachiko::Scripting::DynamicCamera::OnUpdate()
{
	ComponentTransform* transform = game_object->GetTransform();
	
	float delta_time = Time::DeltaTime();
	float distance = math::Distance(_start_point, _end_point);

	_lerp_position += _speed * delta_time;
	
	if (_lerp_position > distance)
	{
		_lerp_position = 0.0f;
	}

	math::float3 lerped_position = math::float3::Lerp(_start_point, _end_point,
		_lerp_position / distance);

	transform->SetGlobalPosition(lerped_position);
}