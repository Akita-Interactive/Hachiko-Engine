#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;

    namespace Scripting
    {
        class AudioManager;
        class PlayerController;
        class CombatManager;
        enum class BugState
        {
            INVALID,
            DEAD,
            IDLE,
            MOVING,
            MOVING_BACK,
            ATTACKING,
            PARASITE,
            SPAWNING,
            PATROL
        };

        class EnemyController : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            EnemyController(GameObject* game_object);
            ~EnemyController() override = default;

            void OnAwake() override;
            void OnStart() override;
            void OnUpdate() override;
            
            BugState GetState() const;

            const Stats* GetStats();
            bool IsAlive() { return _combat_stats->IsAlive(); };
            void SetIsFromGauntlet(bool v) { _is_from_gautlet = v; }
            void RegisterHit(int player_atk, math::float3 direction, float knockback, bool is_from_player);
            void GetParasite();
            void Spawn();
            bool ParasiteDropped() { return _parasite_dropped; };

            void CheckState();
            void ResetEnemy();
            void ResetEnemyPosition();

        private:
            void SpawnController();
            void DeathController();
            void StunController();
            void AttackController();
            void IdleController();

            void Attack();
            void ChasePlayer();
            void GoBack();
            void RecieveKnockback();
            void Stop();

            void MoveInNavmesh();
            void PatrolMovement();
            void DropParasite();
            void DestroyEntity();

            float4x4 GetMeleeAttackOrigin(float attack_range) const;
            bool IsAttacking() const
            {
                return _state == BugState::ATTACKING;
            }

        private:
            Stats* _combat_stats;
            SERIALIZE_FIELD(int, _aggro_range);
            SERIALIZE_FIELD(float, _attack_range);
            SERIALIZE_FIELD(float, _attack_delay);
            SERIALIZE_FIELD(float, _idle_cooldown);
            SERIALIZE_FIELD(float3, _spawn_pos);
            SERIALIZE_FIELD(bool, _spawn_is_initial);
            SERIALIZE_FIELD(GameObject*, _player);
            SERIALIZE_FIELD(GameObject*, _enemy_body);
            SERIALIZE_FIELD(GameObject*, _parasite);
            SERIALIZE_FIELD(GameObject*, _audio_manager_game_object);
            
            SERIALIZE_FIELD(BugState, _state);
            SERIALIZE_FIELD(BugState, _previous_state);
            SERIALIZE_FIELD(bool, _already_in_combat);

            SERIALIZE_FIELD(bool, _is_from_gautlet);

            AudioManager* _audio_manager;
            PlayerController* _player_controller;
            ComponentTransform* transform;
            ComponentAnimation* animation;
            CombatManager* _combat_manager;
            ComponentAudioSource* _audio_source;

            Quat _spawn_rot;

            math::float3 _player_pos;
            math::float3 _target_pos;
            math::float3 _current_pos;
            float _attack_cooldown;

            SERIALIZE_FIELD(float, _attack_animation_duration);
            SERIALIZE_FIELD(float, _attack_animation_timer);

            SERIALIZE_FIELD(bool, _is_ranged_attack);

            bool _parasite_dropped = false;
            bool _is_stunned = false;
            float _stun_time = 0.0f;
            float _acceleration = 0.0f;
            float _speed = 0.0f;
            float _current_idle_cooldown = 0.0f;
            float _enraged = 0.0f;
            float _enemy_dissolve_time = 3.0f;
            float _enemy_dissolving_time_progress = 0.0f;
            const float _enemy_dissolving = 1 / math::Sqrt(_enemy_dissolve_time);
            float _parasite_dissolve_time = 10.0f;
            float _parasite_dissolving_time_progress = 0.0f;
            const float _parasite_dissolving = 1 / math::Sqrt(_parasite_dissolve_time);
            float3 _knockback_pos = float3::zero;
            float _attack_current_delay = 0.f;
            float spawning_time = 2.0f; // Once we have spawning animations this will be calculated by the animation duration
            float spawning_time_passed = 0.f;
            bool _has_spawned = false;
        };
    } // namespace Scripting
} // namespace Hachiko*/
