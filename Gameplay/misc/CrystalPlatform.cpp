#include "scriptingUtil/gameplaypch.h"
#include "CrystalPlatform.h"
#include "constants/Scenes.h"

Hachiko::Scripting::CrystalPlatform::CrystalPlatform(GameObject* game_object)
	: Script(game_object, "CrystalPlatform")
	, _seconds_before_shaking(0.2f)
	, _seconds_shaking(0.8f)
	, _seconds_to_regen(1.f)
	, _shake_intensity(0.1f)
	, _invisible_obstacle(nullptr)
	, _crystal_platform(nullptr)
{
}

void Hachiko::Scripting::CrystalPlatform::OnStart()
{
	player = Scenes::GetPlayer();
	exploding_platform = _crystal_platform->GetComponent<ComponentAnimation>();
	obstacle = _invisible_obstacle->GetComponent<ComponentObstacle>();
	
	ResetTimers();

	if (_invisible_obstacle)
	{
		_invisible_obstacle->SetActive(true);
	}

	_initial_transform = game_object->GetTransform()->GetGlobalMatrix();
}

void Hachiko::Scripting::CrystalPlatform::OnUpdate()
{
	// Platform states always work sequentally, its a behavior loop
	if (GetState() == PlatformState::PLATFORM && exploding_platform->IsAnimationStopped())
	{
		// It needs to happen after animation finishes
		if (obstacle && obstacle->IsInNavMesh())
		{
			obstacle->RemoveObstacle();
		}

		// Check trigger
		if (IsTriggered())
		{
			_state = PlatformState::TRIGGERED;
		}
	}
	else if (GetState() == PlatformState::TRIGGERED)
	{
		before_shake_elapsed += Time::DeltaTime();
		if (before_shake_elapsed >= _seconds_before_shaking)
		{
			_state = PlatformState::SHAKING;
		}
	}
	else if (GetState() == PlatformState::SHAKING) 
	{
		// Is platform shaking
		shake_elapsed += Time::DeltaTime();
		if (shake_elapsed >= _seconds_shaking)
		// Add this line in case shaking is animation = && exploding_platform->AnimationIsStopped()))
		{
			_state = PlatformState::HIDDEN;
		}
		ShakePlatform();
	}
	else if (GetState() == PlatformState::HIDDEN)
	{
		regen_elapsed += Time::DeltaTime();
		if (regen_elapsed >= _seconds_to_regen)
		{
			_state = PlatformState::PLATFORM;
		}
	}

	UpdatePlatformStatus();
}

void Hachiko::Scripting::CrystalPlatform::HidePlatform()
{
	is_shaking = false;
	is_platform_active = true;
	
	if (exploding_platform)
	{
		// Assuming animation was started (?)
		exploding_platform->SendTrigger("isClose");
	}

	if (obstacle && !obstacle->IsInNavMesh())
	{
		obstacle->AddObstacle();
	}
}

void Hachiko::Scripting::CrystalPlatform::RegeneratePlatform()
{
	game_object->GetTransform()->SetGlobalTransform(_initial_transform);

	is_shaking = false;
	is_platform_active = true;

	if (exploding_platform)
	{
		exploding_platform->StartAnimating();
	}

	ResetTimers();
}

void Hachiko::Scripting::CrystalPlatform::UpdatePlatformStatus()
{
	PlatformState platform_state = GetState();
	bool state_changed = platform_state != _previous_state;

	if (!state_changed)
	{
		return;
	}

	_previous_state = platform_state;

	switch (platform_state)
	{
	case PlatformState::INVALID:
		return;
	case PlatformState::HIDDEN:
		HidePlatform();
		break;
	case PlatformState::PLATFORM:
		RegeneratePlatform();
		break;
	case PlatformState::TRIGGERED:
		break;
	case PlatformState::SHAKING:
		break;
	default:
		break;
	}
}

void Hachiko::Scripting::CrystalPlatform::ResetTimers()
{
	before_shake_elapsed = 0.0f;
	shake_elapsed = 0.0f;
	regen_elapsed = 0.f;
}

void Hachiko::Scripting::CrystalPlatform::ShakePlatform()
{
	ComponentTransform* transform = game_object->GetTransform();
	math::float3 current_position = transform->GetGlobalPosition();

	float3 shake_offset = float3::zero;

	shake_offset = GetShakeOffset();
	
	transform->SetGlobalPosition(_initial_transform.Col3(3) + shake_offset);
}

bool Hachiko::Scripting::CrystalPlatform::IsTriggered()
{
	if (!player) return false;

	float distance = player->GetTransform()->GetGlobalPosition().Distance(_invisible_obstacle->GetTransform()->GetGlobalPosition());

	return distance <= obstacle->GetSize().x * obstacle_radius_trigger_percent;
}

float3 Hachiko::Scripting::CrystalPlatform::GetShakeOffset()
{
	if (shake_elapsed < _seconds_shaking)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float x = (r - 0.5f) * shake_magnitude * _shake_intensity;
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float z = (r - 0.5f) * shake_magnitude * _shake_intensity;

		shake_elapsed += Time::DeltaTime();
		shake_magnitude = (1 - (shake_elapsed / _seconds_shaking)) * (1 - (shake_elapsed / _seconds_shaking));
		return float3(x, 0, z);
	}
	else
	{
		return float3::zero;
	}
}
