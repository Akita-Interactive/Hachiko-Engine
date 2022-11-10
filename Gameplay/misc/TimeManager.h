#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    namespace Scripting
    {
        class TimeManager : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            TimeManager(GameObject* new_game_object);
            ~TimeManager() override = default;

            void OnUpdate() override;

            void InterpolateToTimeScale(
                float time_scale = 1.0f, 
                float interpolation_duration = 0.0f, 
                float delay_duration = 0.0f);


        private:
            void ExecuteTimeScaleInterpolation();

        private:
            // If InterpolateToTimeScale is called with delay bigger than zero,
            // this variable denotes the progress of delay between 0 and 1
            // being set to 1 when finished.
            float _delay_progress;
            // Duration of delay that is set by InterpolateToTimeScale.
            float _delay_duration;
            // The current time scale at the point where InterpolateToTimeScale
            // was called.
            float _initial_time_scale;
            // The aimed time scale, given by time_scale input of
            // InterpolateToTimeScale method.
            float _aimed_time_scale;
            // The progress of interpolation, 0 when started, 1 when finished.
            float _interpolation_progress;
            // The duration of the interpolation:
            float _interpolation_duration;
        };
    } // namespace Scripting
} // namespace Hachiko
