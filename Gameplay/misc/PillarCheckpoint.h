#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;
    class ComponentImage;
    class ComponentAudioSource;
    class ComponentPointLight;
    namespace Scripting
    {
        class LevelManager;
        class PillarCheckpoint : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            PillarCheckpoint(GameObject* game_object);
            ~PillarCheckpoint() override = default;

            void OnAwake() override;
            void OnUpdate() override;

            bool IsUsed() { return _used; }
        private:
            bool IsPlayerInRange();
            void ActivateCheckpoint();
        private:
            SERIALIZE_FIELD(GameObject*, _respawn_go);
            SERIALIZE_FIELD(float, _activation_range);
            SERIALIZE_FIELD(GameObject*, _light_go);

            GameObject* _player = nullptr;
            LevelManager* _level_manager = nullptr;
            ComponentAnimation* _animation = nullptr;
            ComponentObstacle* _obstacle = nullptr;
            ComponentAudioSource* _audio_source = nullptr;
            float3 _restart_position;
            bool _used = false;
        };
    } // namespace Scripting
} // namespace Hachiko*/
