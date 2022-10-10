#pragma once
#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;

    namespace Scripting
    {
        class HoverEffect : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            HoverEffect(GameObject* game_object);
            ~HoverEffect() override = default;

            void OnAwake() override;
            void OnUpdate() override;

        private:
            SERIALIZE_FIELD(float, _altitude);
            SERIALIZE_FIELD(float, _magnitude);
            SERIALIZE_FIELD(float, _speed);
            float3 _original_position = float3::zero;
            float _value = 0;
        };
    }
}
