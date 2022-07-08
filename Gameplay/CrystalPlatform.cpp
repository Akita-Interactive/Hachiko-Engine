#include "scriptingUtil/gameplaypch.h"
#include "CrystalPlatform.h"

Hachiko::Scripting::CrystalPlatform::CrystalPlatform(GameObject* game_object)
	: Script(game_object, "CrystalPlatform")
	, _seconds_before_shaking(5.0f)
	, _seconds_shaking(4.0f)
	, _shake_intensity(0.1f)
	, _invisible_obstacle(nullptr)
	, _crystal(nullptr)
	, _crystal_platform(nullptr)
{
}

void Hachiko::Scripting::CrystalPlatform::OnAwake()
{
}

void Hachiko::Scripting::CrystalPlatform::OnStart()
{
	exploding_platform = _crystal_platform->GetComponent<ComponentAnimation>();
	obstacle = _invisible_obstacle->GetComponent<ComponentObstacle>();
	stats = _crystal->GetComponent<Stats>();

	aux_seconds_before_shaking = _seconds_before_shaking;
	aux_seconds_shaking = _seconds_shaking;

	if (_invisible_obstacle && !_invisible_obstacle->IsActive())
	{
		_invisible_obstacle->SetActive(true);
	}

	_initial_transform = game_object->GetTransform()->GetGlobalMatrix();
}

void Hachiko::Scripting::CrystalPlatform::OnUpdate()
{
	// Check if crystal it's alive
	if (!stats->IsAlive() && !is_platform_active)
	{
		_state = PlatformState::PLATFORM;
	}
	
	if (GetState() == PlatformState::PLATFORM && exploding_platform->IsAnimationStopped())
	{
		if (obstacle != nullptr && obstacle->IsInNavMesh())
		{
			obstacle->RemoveObstacle();
		}

		// Wait until start shaking
		if (_seconds_before_shaking <= 0 && !is_shaking && is_platform_active)
		{
			_state = PlatformState::SHAKING;
			_seconds_before_shaking = 0;
		}
		else
		{
			_seconds_before_shaking -= Time::DeltaTime();
		}
	}

	if (GetState() == PlatformState::SHAKING) 
	{
		// Is platform shaking
		if (_seconds_shaking <= 0 && is_shaking)
		// Add this line in case shaking is animation = && exploding_platform->AnimationIsStopped()))
		{
			_state = PlatformState::IDLE;
		}

		ComponentTransform* transform = game_object->GetTransform();
		math::float3 current_position = transform->GetGlobalPosition();

		float3 shake_offset = float3::zero;

		shake_offset = Shake();

		transform->SetGlobalPosition(current_position + shake_offset);
	}

	UpdatePlatformStatus();
}

void Hachiko::Scripting::CrystalPlatform::ShowPlatform()
{
	if (exploding_platform) 
	{
		exploding_platform->StartAnimating();
		is_platform_active = true;
	}
}

void Hachiko::Scripting::CrystalPlatform::RegenerateCrystal()
{
	game_object->GetTransform()->SetGlobalTransform(_initial_transform);

	if (stats && exploding_platform)
	{
		exploding_platform->SendTrigger("isClose");
		stats->_current_hp = 1;
		is_shaking = false;
		is_platform_active = false;

		_seconds_before_shaking = aux_seconds_before_shaking;
		_seconds_shaking = aux_seconds_shaking;

		if (obstacle != nullptr && !obstacle->IsInNavMesh())
		{
			obstacle->AddObstacle();
		}
	}
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
	case PlatformState::IDLE:
		RegenerateCrystal();
		break;
	case PlatformState::PLATFORM:
		ShowPlatform();
		break;
	case PlatformState::SHAKING:
		is_shaking = true;
		break;
	default:
		break;
	}
}

float3 Hachiko::Scripting::CrystalPlatform::Shake()
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
		_seconds_shaking = 0.0f;
		shake_elapsed = 0.0f;
		return float3::zero;
	}
}
