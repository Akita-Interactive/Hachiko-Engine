#pragma once

#include "resources/ResourceAnimation.h"

#include <Math.h>

#include <vector>

namespace Hachiko
{
    class AnimationController
    {
    public:

        enum class State
        {
            PLAYING, // While the animation is playing
            PAUSED, // If the animation is paused
            STOPPED, // When the animation is finished
            UNSET
        };

        struct Instance
        {
            ResourceAnimation* current_animation = nullptr;
            unsigned time = 0;
            bool loop = true;
            float speed = 1.0;

            Instance* previous = nullptr;
            unsigned fade_duration = 0;
            unsigned fade_time = 0;
        };

    public:

        AnimationController();
        ~AnimationController();

        void Play(ResourceAnimation* current_animation, bool loop, unsigned fade_time);
        void Update(unsigned elapsed, bool reverse);
        void Stop();

        bool GetTransform(Instance* instance, const std::string& channel_name, math::float3& position, Quat& rotation) const;

        Instance* GetCurrentInstance() 
        {
            return current;
        }

        State GetCurrentState() const
        {
            return current_state;
        }

        void SetCurrentState(State new_current_state)
        {
            current_state = new_current_state;
        }

    private:
        Instance* current = nullptr; // move to private

        // INTERPOLATION
        float Interpolate(float first, float second, float lambda);
        float3 Interpolate(float3& first, float3& second, float lambda) const; 
        Quat Interpolate(Quat& first, Quat& second, float lambda) const;

        // MANAGE INSTANCES
        void UpdateInstance(Instance* instance, unsigned elapsed, bool reverse);
        void ReleaseInstance(Instance* instance);

        State current_state = State::UNSET;
    };
} // namespace Hachiko
