#include "scriptingUtil/gameplaypch.h"
#include "PlayerController.h"
#include "Scenes.h"

#include <components/ComponentTransform.h>
#include <components/ComponentCamera.h>
#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::PlayerController::PlayerController(GameObject* game_object)
	: Script(game_object, "PlayerController")
	, _movement_speed(0.0f)
	, _dash_indicator(nullptr)
	, _dash_duration(0.0f)
	, _dash_distance(0.0f)
	, _dash_progress(0.0f)
	, _dash_cooldown(0.0f)
	, _is_dashing(false)
	, _dash_start(math::float3::zero)
	, _dash_direction(math::float3::zero)
	, _should_rotate(false)
	, _rotation_progress(0.0f)
	, _rotation_duration(0.0f)
	, _rotation_target(math::Quat::identity)
	, _rotation_start(math::Quat::identity)
{
}

void Hachiko::Scripting::PlayerController::OnAwake()
{
	_dash_distance = 5.0f;
	_dash_duration = 0.15f;
	_dash_cooldown = 0.25f;
	_movement_speed = 10.0f;
	_rotation_duration = 0.075f;
	_dash_indicator = game_object->GetFirstChildWithName("DashIndicator");
}

void Hachiko::Scripting::PlayerController::OnUpdate()
{
	ComponentTransform* transform = game_object->GetTransform();
	math::float3 current_position = transform->GetGlobalPosition();

	// Handle all the input:
	HandleInput(current_position);

	// Dash:
	Dash(current_position);

	// Rotate player to the necessary direction:
	//Rotate(transform, current_position);

	// Move the dash indicator:
	//MoveDashIndicator(current_position);

	// Attack:
	Attack(transform, current_position);

	// Apply the position:
	transform->SetGlobalPosition(current_position);
	transform->LookAtTarget(GetRaycastPosition(current_position));

	// Instantiate GameObject in current scene test:
	SpawnGameObject();

} 

math::float3 Hachiko::Scripting::PlayerController::GetRaycastPosition(
	const math::float3& current_position) const
{
	const math::Plane plane(math::float3(0.0f, current_position.y, 0.0f),
		math::float3(0.0f, 1.0f, 0.0f));

	const math::float2 mouse_position_view =
		ComponentCamera::ScreenPositionToView(Input::GetMousePosition());

	const math::LineSegment ray = Debug::GetRenderingCamera()->Raycast(
		mouse_position_view.x, mouse_position_view.y);

	return plane.ClosestPoint(ray);
}

void Hachiko::Scripting::PlayerController::MoveDashIndicator(
	const math::float3& current_position) const
{
	const math::float3 mouse_world_position = 
		GetRaycastPosition(current_position);

	math::float3 direction = mouse_world_position- current_position;
	direction.Normalize();

	_dash_indicator->GetTransform()->SetGlobalPosition(current_position
		+ direction);
	_dash_indicator->GetTransform()->SetGlobalRotationEuler(
		float3(90.0f, 0.0f, 0.0f));
}

void Hachiko::Scripting::PlayerController::SpawnGameObject() const
{
	static int times_hit_g = 0;
	
	if (!Input::GetKeyDown(Input::KeyCode::KEY_G))
	{
		return;
	}

	std::string name = "GameObject ";
		
	name += std::to_string(times_hit_g);
		
	times_hit_g++;

	GameObject* created_game_object = GameObject::Instantiate();

	created_game_object->SetName(name);
}

void Hachiko::Scripting::PlayerController::Attack(ComponentTransform* transform,
	const math::float3& current_position)
{
	// For now this only makes player look at to the direction to the mouse
	// on left mouse button is clicked, can be used as a base to build the 
	// actual combat upon.
	// TODO: Improve this method and implement actual attacking.
	
	if (_is_dashing || !Input::GetMouseButton(Input::MouseButton::LEFT))
	{
		return;
	}

	// Make the player look the mouse:
	
}

void Hachiko::Scripting::PlayerController::Dash(math::float3& current_position)
{
	// This is a rather primitive dash that should be used as a base to build 
	// a more polished one. Some related issues needed to be fixed are:
	// 1. It should have a cooldown.
	// 2. As we use the camera that has PlayerCamera script is attached to now,
	//    while the camera is lerping with a delay, the corresponding world 
	//    position of mouse position changes as the camera approaches its final
	//    position, that causes dash to seem kind of unstable.
	// 3. Instead of approaching to _dash_end linearly, dash must have some sort
	//    of an acceleration.
	// TODO: Address the issues above. 

	if (!_is_dashing)
	{
		return;
	}

	_dash_progress += Time::DeltaTime() / _dash_duration;
	_dash_progress = _dash_progress > 1.0f ? 1.0f : _dash_progress;

	_is_dashing = (_dash_progress < 1.0f);

	const math::float3 _dash_end = 
		_dash_start + _dash_direction * _dash_distance;


	current_position = math::float3::Lerp(_dash_start, _dash_end, 
		_dash_progress);
}

void Hachiko::Scripting::PlayerController::Rotate(
	ComponentTransform* transform, const math::float3& current_position)
{
	// If the player position is changed, check if rotation is needed:
	if (!current_position.Equals(transform->GetGlobalPosition()))
	{
		// Get the position player should be looking at:
		math::float3 look_at_position = current_position;
		look_at_position.y = transform->GetGlobalPosition().y;

		// Calculate the direction player should be looking at:
		math::float3 look_at_direction = 
			(transform->GetGlobalPosition() - look_at_position);
		look_at_direction.Normalize();

		// Get the rotation player is going to have:
		const math::Quat target_rotation = 
			transform->SimulateLookAt(look_at_direction);

		// If rotation is gonna be changed fire up the rotating process:
		if (!target_rotation.Equals(transform->GetGlobalRotation()))
		{
			// Reset all the variables related to rotation lerp:
			_rotation_progress = 0.0f;
			_rotation_target = target_rotation;
			_rotation_start = transform->GetGlobalRotation();
			_should_rotate = true;
		}
	}
	
	// Smoothly rotate to the new direction of the player in _rotation_duration
	// amount of seconds:
	if (_should_rotate)
	{
		_rotation_progress += Time::DeltaTime() / _rotation_duration;
		_rotation_progress = _rotation_progress > 1.0f 
			? 1.0f 
			: _rotation_progress;

		transform->SetGlobalRotation(
			Quat::Lerp(_rotation_start, _rotation_target, _rotation_progress));

		_should_rotate = _rotation_progress < 1.0f;

		if (!_should_rotate)
		{
			_rotation_progress = 0.0f;
			_rotation_target = math::Quat::identity;
			_rotation_start = math::Quat::identity;
			_should_rotate = false;
		}
	}
}

void Hachiko::Scripting::PlayerController::HandleInput(
	math::float3& current_position)
{
	// Ignore the inputs if engine camera input is taken:
	if (Input::GetMouseButton(Input::MouseButton::RIGHT))
	{
		return;
	}

	// Dashing locks player from submitting new commands on input:
	if (_is_dashing)
	{
		return;
	}

	const float velocity = _movement_speed * Time::DeltaTime();
	const math::float3 delta_x = math::float3::unitX * velocity;
	const math::float3 delta_y = math::float3::unitY * velocity;
	const math::float3 delta_z = math::float3::unitZ * velocity;

	if (Input::GetKey(Input::KeyCode::KEY_W))
	{
		current_position -= delta_z;
	}
	else if (Input::GetKey(Input::KeyCode::KEY_S))
	{
		current_position += delta_z;
	}

	if (Input::GetKey(Input::KeyCode::KEY_D))
	{
		current_position += delta_x;
	}
	else if (Input::GetKey(Input::KeyCode::KEY_A))
	{
		current_position -= delta_x;
	}

	if (Input::GetKey(Input::KeyCode::KEY_Q))
	{
		current_position += delta_y;
	}
	else if (Input::GetKey(Input::KeyCode::KEY_E))
	{
		current_position -= delta_y;
	}

	if (Input::GetKeyDown(Input::KeyCode::KEY_SPACE))
	{

		_is_dashing = true;
		_dash_progress = 0.0f;
		_dash_start = current_position;

		const math::float3 dash_end = GetRaycastPosition(
			_dash_start);
		
		_dash_direction = dash_end - _dash_start;
		_dash_direction.Normalize();

	}

}