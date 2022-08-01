#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    namespace Scripting
    {
        class Stats : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            Stats(GameObject* game_object);
            ~Stats() override = default;

            void OnAwake() override;

            void ReceiveDamage(int _damage);
            void Heal(int _health);
            void SetHealth(int health);

            void ChangeWeapon(int attk_pwr, float attk_cd, float attk_range)
            {
                _attack_power = attk_pwr;
                _attack_cd = attk_cd;
                _attack_range = attk_range;
            }

            enum AttackType
            {
                CONE = 0,
                RECTANGLE = 1,
                CIRCLE = 2
            };

            bool IsAlive();
        public:
            SERIALIZE_FIELD(int, _attack_power);
            SERIALIZE_FIELD(float, _attack_knockback);
            SERIALIZE_FIELD(float, _attack_cd);
            SERIALIZE_FIELD(float, _attack_range);
			SERIALIZE_FIELD(float, _attack_width);
            SERIALIZE_FIELD(float, _move_speed);
            SERIALIZE_FIELD(int, _max_hp);
            SERIALIZE_FIELD(int, _attack_type);

            SERIALIZE_FIELD(int, _current_hp);

        };
    
		
    } // namespace Scripting
} // namespace Hachiko*/