#include "scriptingUtil/gameplaypch.h"
#include "PlayerController.h"
#include "Scenes.h"

#include <components/ComponentTransform.h>
#include <components/ComponentCamera.h>
#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::PlayerController::PlayerController(GameObject* game_object)
	: Script(game_object, "PlayerController")
	, _movement_speed(0.0f)
	, _dash_distance(0.0f)
	, _dash_duration(0.0f)
	, _is_dashing(false)
	, _dash_progress(0.0f)
	, _dash_start(math::float3::zero)
	, _rotation_speed(0.0f)
	, _is_falling(false)
	, _original_y(0.0f)
	, _speed_y(0.0f)
	, _starting_position(math::float3::zero)
{
}

void Hachiko::Scripting::PlayerController::OnAwake()
{
	_dash_distance = 1.0f;
	_dash_duration = 0.10f;
	_movement_speed = 10.0f;
	_rotation_speed = 2.5f;

	_original_y = game_object->GetTransform()->GetGlobalPosition().y;
	_speed_y = 0.0f;
	
	_starting_position = math::float3(0.0f, 1.0f, 0.0f);
}

void Hachiko::Scripting::PlayerController::OnStart()
{
}

void Hachiko::Scripting::PlayerController::OnUpdate()
{
	// Ignore the inputs if engine camera input is taken:
	if (Input::GetMouseButton(Input::MouseButton::RIGHT))
	{
		return;
	}

	ComponentTransform* transform = game_object->GetTransform();
	
	math::float3 current_position = transform->GetGlobalPosition();
	math::float3 current_front = transform->GetFront();
	math::float3 current_right = transform->GetRight();

	float delta_time = Time::DeltaTime();
	float velocity = _movement_speed * delta_time;

	GameObject* raycast_hit = SceneManagement::Raycast(
		current_position - math::float3(0.0f, 1.0f, 0.0f), current_position);

	if (raycast_hit == nullptr || raycast_hit->name == "Player") 
	{
		_is_falling = true;
	}

	if (raycast_hit != nullptr && raycast_hit->name == "Goal")
	{
		// TODO: Uncomment this in the next PR after adding the new scenes with
		// YAML based serialization.
		// 
		// SceneManagement::SwitchScene(Scenes::WIN);
	}

	// TODO: Delete this in the next PR after adding the new scenes with
	// YAML based serialization. For now, player is not falling by default.
	_is_falling = false;

	if (!_is_dashing && !_is_falling)
	{
		if (Input::GetKey(Input::KeyCode::KEY_W))
		{
			current_position -= math::float3(0, 0, 1) * velocity;
		}
		else if (Input::GetKey(Input::KeyCode::KEY_S))
		{
			current_position += math::float3(0, 0, 1) * velocity;
		}

		if (Input::GetKey(Input::KeyCode::KEY_D))
		{
			current_position += math::float3(1, 0, 0) * velocity;
		}
		else if (Input::GetKey(Input::KeyCode::KEY_A))
		{
			current_position -= math::float3(1, 0, 0) * velocity;
		}

		if (Input::GetKeyDown(Input::KeyCode::KEY_SPACE))
		{
			_is_dashing = true;
			_dash_progress = 0.0f;
			_dash_start = current_position;
		}
	}

	if (_is_dashing)
	{
		_dash_progress += delta_time / _dash_duration;
		_dash_progress = _dash_progress > 1.0f ? 1.0f : _dash_progress;

		_is_dashing = (_dash_progress < 1.0f);

		math::float3 _dash_end = current_position + 
			current_front * _dash_distance;

		current_position = math::float3::Lerp(_dash_start, _dash_end, 
			_dash_progress);
	}

	if (_is_falling)
	{
		_speed_y += 25.0f * delta_time;
		current_position.y -= _speed_y * delta_time;
	}

	// Reset falling:
	if (Input::GetKeyDown(Input::KeyCode::KEY_P))
	{
		if (_is_falling)
		{
			_speed_y = 0;
			current_position.y = _original_y;
		}
		_is_falling = !_is_falling;
	}

	// TODO: Uncomment this in the next PR after adding the new scenes with
	// YAML based serialization.
	/*if (current_position.y < -20)
	{
		_speed_y = 0;
		current_position = _starting_position;
		_is_falling = false;

		SceneManagement::SwitchScene(Scenes::LOSE);
	}*/

	// Apply the position:
	transform->SetGlobalPosition(current_position);

	if (!_is_dashing)
	{
		math::Plane plane(
			math::float3(0.0f, current_position.y, 0.0f),
			math::float3(0.0f, 1.0f, 0.0f));

		math::float2 mouse_position_view = 
			ComponentCamera::ScreenPositionToView(Input::GetMousePosition());
		
		math::LineSegment ray = Debug::GetEditorCamera()->Raycast(
			mouse_position_view.x, mouse_position_view.y);

		math::float3 intersection_position = plane.ClosestPoint(ray);

		// Make the player look the mouse:
		transform->LookAtTarget(intersection_position);
	}
}