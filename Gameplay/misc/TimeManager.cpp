#include "scriptingUtil/gameplaypch.h"
#include "TimeManager.h"

Hachiko::Scripting::TimeManager::TimeManager(GameObject* new_game_object)
    : Script(new_game_object, "TimeManager")
    , _delay_progress(1)
    , _delay_duration(0)
    , _initial_time_scale(0)
    , _aimed_time_scale(0)
    , _interpolation_progress(1)
    , _interpolation_duration(0) {}

void Hachiko::Scripting::TimeManager::OnUpdate()
{
    ExecuteTimeScaleInterpolation();
}

void Hachiko::Scripting::TimeManager::InterpolateToTimeScale(
    const float time_scale,
    const float interpolation_duration,
    const float delay_duration)
{
    constexpr float default_time_scale = 1.0f;

    _delay_progress = delay_duration <= 0.0f ? 1.0f : 0.0f;
    _delay_duration = delay_duration < 0.0f ? 0.0f : delay_duration;

    _initial_time_scale = Time::GetTimeScale();
    _aimed_time_scale = time_scale >= 0.0f ? time_scale : default_time_scale;

    _interpolation_progress = interpolation_duration <= 0.0f ? 1.0f : 0.0f;
    _interpolation_duration = interpolation_duration > 0.0f
        ? interpolation_duration
        : 0.0f;
}

void Hachiko::Scripting::TimeManager::ExecuteTimeScaleInterpolation()
{
    // If the interpolation is ended, halt:
    if (_interpolation_progress >= 1.0f)
    {
        return;
    }

    // Wait for the delay:
    if (_delay_progress < 1.0f)
    {
        // The case of division by zero is handled inside
        // InterpolateToTimeScale:
        _delay_progress += Time::DeltaTime() / _delay_duration;
        _delay_progress = _delay_progress > 1.0f ? 1.0f : _delay_progress;

        return;
    }

    // The case of division by zero is handled inside InterpolateToTimeScale:
    _interpolation_progress += Time::DeltaTime() / _interpolation_duration;
    _interpolation_progress = _interpolation_progress > 1.0f
        ? 1.0f
        : _interpolation_progress;

    const float current_time_scale = math::Lerp(
        _initial_time_scale,
        _aimed_time_scale,
        _interpolation_progress);

    Time::SetTimeScale(current_time_scale);
}
