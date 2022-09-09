#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;

    namespace Scripting
    {
        class LaserController : public Script
        {
            SERIALIZATION_METHODS(false)

            enum State
            {
                ACTIVE = 0,
                ACTIVATING = 1,
                INACTIVE = 2
            };

            // Methods
        public:
            LaserController(GameObject* game_object);
            ~LaserController() override = default;

            void OnAwake() override;
            void OnUpdate() override;

            void ChangeState(State new_state);

        private:
            void AdjustLength();
            void CheckPlayerCollision() const;

            // Variables
        public:
            SERIALIZE_FIELD(GameObject*, _laser);
            SERIALIZE_FIELD(float, _max_length);
            SERIALIZE_FIELD(float, _max_scale);
            SERIALIZE_FIELD(float, _activation_time);
            SERIALIZE_FIELD(float, _damage);
            SERIALIZE_FIELD(bool, _spin_movement);
            SERIALIZE_FIELD(float, _spin_speed);
            SERIALIZE_FIELD(bool, _horizonal_movement);
            SERIALIZE_FIELD(float, _movement_speed);
            SERIALIZE_FIELD(GameObject*, _movement_target);
            SERIALIZE_FIELD(bool, _toggle_activation);
            SERIALIZE_FIELD(float, _toggle_active_time);
            SERIALIZE_FIELD(float, _toggle_inactive_time);

        private:
            State _state = ACTIVE;
            float _elapsed_time = 0.0f;
            float _length = 0.0f;
            float _scale = 0.2f;

            float3 _initial_position = float3::zero;
            bool _movement_forward_direction = true;
            float _movement_position = 0.0f;

            GameObject* _terrain = nullptr;
            GameObject* _player = nullptr;
            ComponentAudioSource* _audio_source;
        };
    }
}
