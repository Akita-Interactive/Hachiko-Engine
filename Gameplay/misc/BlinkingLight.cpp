#include "scriptingUtil/gameplaypch.h"
#include "misc/BlinkingLight.h"

Hachiko::Scripting::BlinkingLight::BlinkingLight(Hachiko::GameObject* game_object)
	: Script(game_object, "BlinkingLight")
	, _use_perlin_noise(false)
	, _min_radius(10.0f)
	, _max_radius(250.0f)
	, _radius_amplitude(1.0f)
	, _min_duration(0.01f)
    , _max_duration(0.5f)
    , _duration_amplitude(1.0f)
    , _min_intensity(50.0f)
    , _max_intensity(200.0f)
    , _intensity_amplitude(1.0f)
    , _starting_intensity(0.0f)
    , _starting_radius(0.0f)
    , _current_intensity(0.0f)
    , _current_duration(0.0f)
	, _current_radius(0.0f)
    , _next_duration(0.0f)
    , _next_intensity(0.0f)
    , _next_radius(0.0f)
    , _point_light(nullptr)
	, _perlin_noise(PerlinNoise1D())
{
}

void Hachiko::Scripting::BlinkingLight::OnAwake()
{
	_point_light = game_object->GetComponent<ComponentPointLight>();

	// Trigger random generation:
	_next_duration = RandomUtil::RandomBetween(_min_duration, _max_duration);
	_current_duration = _next_duration;

	_next_intensity = RandomUtil::RandomBetween(_min_intensity, _max_intensity);
	_current_intensity = _next_intensity;
	_starting_intensity = _current_intensity;

	_next_radius = RandomUtil::RandomBetween(_min_radius, _max_radius);
	_current_radius = _next_radius;
	_starting_radius = _current_radius;
}

void Hachiko::Scripting::BlinkingLight::OnUpdate()
{
	// Tick the duration clock:
	_current_duration = _current_duration + Time::DeltaTime(); 
	_current_duration = _current_duration > _next_duration ? _next_duration : _current_duration;
	
	// Mark the flag if the counter is over:
	const bool trigger_random = (_current_duration == _next_duration);

	// Lerp intensity:
	_current_intensity = math::Lerp(_starting_intensity, _next_intensity, 
		_current_duration / _next_duration);
	_current_intensity = _current_intensity < _min_intensity
		? _min_intensity
		: (_current_intensity > _max_intensity ? _max_intensity : _current_intensity);

	// Lerp radius:
	_current_radius = math::Lerp(_starting_radius, _next_radius,
		_current_duration / _next_duration);
	_current_radius = _current_radius < _min_radius
		? _min_radius
		: (_current_radius > _max_radius ? _max_radius : _current_radius);

	// Apply values:
	_point_light->intensity = _current_intensity;
	_point_light->radius = _current_radius;

	// If the duration counter still haven't reached the _next_duration value, 
	// terminate the method:
	if (!trigger_random)
	{
		return;
	}

	// Reset the counter:
	_current_duration = 0.0f;

	// Set the starting values to be previous target values:
	_starting_intensity = _next_intensity;
	_starting_radius = _next_radius;

	// Perlin noise generates closer values to the previous values, and can be
	// used if subtle blinking is aimed:
	if (_use_perlin_noise)
	{
		_next_intensity = _perlin_noise.RandomRange(_next_intensity * _intensity_amplitude, _min_intensity, _max_intensity);
		_next_duration = _perlin_noise.RandomRange(_next_duration * _duration_amplitude, _min_duration, _max_duration);
		_next_radius = _perlin_noise.RandomRange(_next_radius * _radius_amplitude, _min_radius, _max_radius);
	}
	// Good old normally distributed pseudo-random number generation:
	else
	{
		_next_intensity = RandomUtil::RandomBetween(_min_intensity, _max_intensity);
		_next_duration = RandomUtil::RandomBetween(_min_duration, _max_duration);
		_next_radius = RandomUtil::RandomBetween(_min_radius, _max_radius);
	}
}