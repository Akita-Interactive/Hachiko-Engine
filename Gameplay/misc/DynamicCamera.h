#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;

    namespace Scripting
    {
        class PlayerCamera;
        class LevelManager;

        class DynamicCamera : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            DynamicCamera(GameObject* game_object);
            ~DynamicCamera() override = default;

            void StartCameraTravel() { _playing = true; }
            void StopCameraTravel() { _playing = false; }
            void SetCurrentShot(unsigned target_idx);

            void OnAwake() override;
            void OnUpdate() override;

        private:
            void OnFinish();
            void MoveToCurrentTargetShot();
            void GetNextTargetShot();
            void SetTravelTarget(unsigned target_idx);
            void ComputeTransition(unsigned target_idx);
            SERIALIZE_FIELD(float, _speed);
            SERIALIZE_FIELD(GameObject*, _enable_after);

            GameObject* _camera_go = nullptr;
            std::vector<GameObject*> _camera_positions;
            ComponentTransform* _camera_transform{};
            ComponentCamera* _camera = nullptr;
            PlayerCamera* _player_camera = nullptr;
            LevelManager* _level_manager = nullptr;

            unsigned _current_target = 0;
            float _transition_progress = 0.f;
            float _transition_duration = 0.f;
            float3 _start_pos = float3::zero;
            Quat _start_rot{};
            bool _playing = false;
        };
    } // namespace Scripting
} // namespace Hachiko
