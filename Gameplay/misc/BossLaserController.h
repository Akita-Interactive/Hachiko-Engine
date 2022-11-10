#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;

    namespace Scripting
    {
        class BossLaserController : public Script
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
            BossLaserController(GameObject* game_object);
            ~BossLaserController() override = default;

            void OnAwake() override;
            void OnUpdate() override;

            void ChangeState(State new_state);

        private:
            void RotateTowardsPlayer();
            void AdjustLength();
            void CheckPlayerCollision() const;

            // Variables
        public:
            SERIALIZE_FIELD(GameObject*, _laser);
            SERIALIZE_FIELD(float, _max_length);
            SERIALIZE_FIELD(float, _max_scale);
            SERIALIZE_FIELD(float, _activation_time);
            SERIALIZE_FIELD(float, _damage);
            SERIALIZE_FIELD(bool, _track_if_active);
            SERIALIZE_FIELD(float, _tracking_speed);
            SERIALIZE_FIELD(bool, _toggle_activation);
            SERIALIZE_FIELD(float, _toggle_active_time);
            SERIALIZE_FIELD(float, _toggle_inactive_time);

        private:
            State _state = ACTIVE;
            float _elapsed_time = 0.0f;
            float _length = 0.0f;
            float _scale = 0.2f;

            GameObject* _terrain = nullptr;
            GameObject* _player = nullptr;
            ComponentAudioSource* _audio_source;
        };
    }
}
