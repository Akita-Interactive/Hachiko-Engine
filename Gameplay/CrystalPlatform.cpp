#include "scriptingUtil/gameplaypch.h"

#include "core/GameObject.h"

#include "CrystalPlatform.h"

#include <resources/ResourceAnimation.h>


Hachiko::Scripting::CrystalPlatform::CrystalPlatform(GameObject* game_object)
	: Script(game_object, "CrystalPlatform")
	, _seconds_before_shaking(5.0f)
	, _seconds_shaking(4.0f)
	, _shake_intensity(0.5f)
	, _invisible_obstacle(nullptr)
	, _crystal(nullptr)
	, _crystal_platform(nullptr)
{
}

void Hachiko::Scripting::CrystalPlatform::OnAwake()
{}

void Hachiko::Scripting::CrystalPlatform::OnStart()
{
	exploding_platform = _crystal_platform->GetComponent<ComponentAnimation>();
	obstacle = _invisible_obstacle->GetComponent<ComponentObstacle>();
	stats = _crystal->GetComponent<Stats>();

	if (_invisible_obstacle && !_invisible_obstacle->IsActive())
	{
		_invisible_obstacle->SetActive(true);
	}
}

void Hachiko::Scripting::CrystalPlatform::OnUpdate()
{
	// Check if crystal it's alive
	if (!stats->IsAlive() && !is_platform_active)
	{
		_state = PlatformState::PLATFORM;
	}
	
	if (GetState() == PlatformState::PLATFORM)
	// && exploding_platform->AnimationIsStopped()) Add this when we have animations
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
			_seconds_shaking = 0;
		}
		else
		{
			_seconds_shaking -= Time::DeltaTime();
		}
	}


	UpdatePlatformStatus();
}

void Hachiko::Scripting::CrystalPlatform::ShowPlatform()
{
	is_platform_active = true;
	if (exploding_platform) 
	{
		exploding_platform->StartAnimating();
		// is_platform_active = true; // TODO: When we have animation this should be heare inside aniamtion
	}
}

void Hachiko::Scripting::CrystalPlatform::RegenerateCrystal()
{
	if (stats) 
	// && exploding_platform
	{
		// exploding_platform->SendTrigger("isRegenereted");
		stats->_current_hp = 1;
		is_shaking = false;
		is_platform_active = false;

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
		Shaking();
		break;
	default:
		break;
	}
}

void Hachiko::Scripting::CrystalPlatform::Shaking()
{
	is_shaking = true;

	/*float3 shake_offset = float3::zero;

	shake_offset = Shake();

	_crystal_platform->GetComponent<ComponentTransform>()->SetGlobalPosition(
		game_object->GetComponent<ComponentTransform>()->GetGlobalPosition()
		+ shake_offset);*/
}


float3 Hachiko::Scripting::CrystalPlatform::Shake()
{
	if (shake_elapsed < _seconds_shaking)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float x = (r - 0.5f) * shake_magnitude * 0.2f;
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float z = (r - 0.5f) * shake_magnitude * 0.2f;

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
