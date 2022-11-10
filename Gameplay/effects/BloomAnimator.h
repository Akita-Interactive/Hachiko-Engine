#pragma once
#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    class ComponentMeshRenderer;

    namespace Scripting
    {
        class BloomAnimator : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            BloomAnimator(GameObject* game_object);
            ~BloomAnimator() override = default;

            void OnAwake() override;
            void OnUpdate() override;

            void AnimateBloomManually(
                float4 initial_emissive_color,
                float target_intensity, 
                float duration, 
                bool should_return_to_automatic_mode = false);
            void SetShouldAnimate(bool should_animate);
            void RefreshAsAutomatic();

        private:
            SERIALIZE_FIELD(GameObject*, _bloom_target);
            SERIALIZE_FIELD(bool, _is_automatic);
            SERIALIZE_FIELD(bool, _is_randomized);
            SERIALIZE_FIELD(float, _randomized_duration_min);
            SERIALIZE_FIELD(float, _randomized_duration_max);
            SERIALIZE_FIELD(float, _automatic_lerp_duration);
            SERIALIZE_FIELD(float4, _initial_emissive_color);

            float _initial_intensity;
            float _target_intensity;
            float _current_intensity;
            float _used_lerp_duration;
            float _lerp_progress;
            bool _should_return_to_automatic_mode;
            bool _should_animate;
        };
    }
}
