#include "scriptingUtil/gameplaypch.h"
#include "BloomAnimator.h"

Hachiko::Scripting::BloomAnimator::BloomAnimator(GameObject* game_object)
	: Script(game_object, "BloomAnimator")
	, _bloom_target(nullptr)
	, _is_automatic(true)
	, _is_randomized(false)
	, _is_recursive(true)
	, _randomized_duration_min(0.2f)
	, _randomized_duration_max(3.0f)
	, _automatic_lerp_duration(1.0f)
	, _initial_intensity(0.0f)
	, _target_intensity(1.0f)
	, _current_intensity(0.0f)
	, _used_lerp_duration(1.0f)
	, _lerp_progress(0.0f)
	, _should_return_to_automatic_mode(true)
	, _should_animate(true)
{
}

void Hachiko::Scripting::BloomAnimator::OnAwake()
{
	if (!_bloom_target && game_object->GetComponent(Type::MESH_RENDERER))
	{
		_bloom_target = game_object;
	}

	if (!_bloom_target)
	{
		return;
	}

	_affected_game_objects.push_back(_bloom_target);

	if (!_is_recursive)
	{
		return;
	}

	const std::vector<Component*> affected_meshes = 
		_bloom_target->GetComponentsInDescendants(Type::MESH_RENDERER);

	for (Component* affected_mesh : affected_meshes)
	{
		_affected_game_objects.push_back(affected_mesh->GetGameObject());
	}
}

void Hachiko::Scripting::BloomAnimator::OnUpdate()
{
	if (!_should_animate || _affected_game_objects.empty())
	{
		return;
	}

	_lerp_progress += Time::DeltaTime() / _used_lerp_duration;
	_lerp_progress = _lerp_progress > 1.0f ? 1.0f : _lerp_progress;

	_current_intensity = 
		math::Lerp(_initial_intensity, _target_intensity, _lerp_progress);

	const float4 emissive_color(
		_initial_emissive_color.xyz(), 
		_current_intensity);

	for (GameObject* bloom_target : _affected_game_objects)
	{
		bloom_target->ChangeEmissiveColor(emissive_color, false, true);
	}

	if (_lerp_progress < 1.0f)
	{
		return;
	}

	_lerp_progress = 0.0f;

	if (_is_automatic || _should_return_to_automatic_mode)
	{
		std::swap(_target_intensity, _initial_intensity);
		_current_intensity = _initial_intensity;

		_used_lerp_duration = _is_randomized
			? RandomUtil::RandomBetween(
				_randomized_duration_min, _randomized_duration_max)
			: _automatic_lerp_duration;

		_is_automatic = true;
	}
	else
	{
		_current_intensity = _initial_intensity = 0.0f;
		_target_intensity = 1.0f;
		_should_animate = false;
	}
}

void Hachiko::Scripting::BloomAnimator::AnimateBloomManually(
	const float4 initial_emissive_color,
	const float target_intensity, 
	const float duration,
	const bool should_return_to_automatic_mode)
{
	SetShouldAnimate(true);

	_initial_emissive_color = initial_emissive_color;

	_initial_intensity = initial_emissive_color.w;

	_used_lerp_duration = duration;

	_target_intensity = target_intensity;

	_should_return_to_automatic_mode = should_return_to_automatic_mode;
}

void Hachiko::Scripting::BloomAnimator::SetShouldAnimate(bool should_animate)
{
	_should_animate = should_animate;
}

void Hachiko::Scripting::BloomAnimator::RefreshAsAutomatic()
{
	_is_automatic = true;
	_current_intensity = _initial_intensity = 0.0f;
	_target_intensity = 1.0f;
	_should_animate = false;
}
