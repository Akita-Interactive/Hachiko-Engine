#pragma once
#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;

    namespace Scripting
    {
        class RotationEffect : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            RotationEffect(GameObject* game_object);
            ~RotationEffect() override = default;

            void OnUpdate() override;

        private:
            SERIALIZE_FIELD(bool, _rotate_x);
            SERIALIZE_FIELD(bool, _rotate_y);
            SERIALIZE_FIELD(bool, _rotate_z);
            SERIALIZE_FIELD(float, _rotation_speed);
        };
    }
}
