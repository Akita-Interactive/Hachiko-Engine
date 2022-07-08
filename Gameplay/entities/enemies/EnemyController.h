#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"
#include "entities/player/CombatManager.h"

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;
    class ComponentParticleSystem;
    class ComponentBillboard;

    namespace Scripting
    {
        class AudioManager;
        class PlayerController;
        class CombatVisualEffectsPool;

        class PlayerCamera;

        enum class EnemyType
        {
            BEETLE,
            WORM
        };
        enum class BugState
        {
            INVALID,
            DEAD,
            IDLE,
            ATTACKING,
            PARASITE,
            SPAWNING,
            // Beetle unique states
            MOVING,
            MOVING_BACK,
            PATROL,
            // Worm unique states
            HIDEN,
            HIT
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
            void RegisterHit(int player_atk, math::float3 direction, float knockback, bool is_from_player, bool is_ranged);
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

            void WormSpit();

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
            SERIALIZE_FIELD(bool, _worm);
            SERIALIZE_FIELD(int, _aggro_range);
            SERIALIZE_FIELD(float, _attack_range);
            SERIALIZE_FIELD(float, _attack_delay);
            SERIALIZE_FIELD(float, _idle_cooldown);
            SERIALIZE_FIELD(float, _spawning_time);
            SERIALIZE_FIELD(float3, _spawn_pos);
            SERIALIZE_FIELD(bool, _spawn_is_initial);
            
            SERIALIZE_FIELD(GameObject*, _enemy_body);
            SERIALIZE_FIELD(GameObject*, _parasite);
            SERIALIZE_FIELD(GameObject*, _blood_trail);
            SERIALIZE_FIELD(GameObject*, _small_dust);
            SERIALIZE_FIELD(GameObject*, _big_dust);
            SERIALIZE_FIELD(GameObject*, _attack_zone);
            SERIALIZE_FIELD(GameObject*, _inner_indicator);
            SERIALIZE_FIELD(GameObject*, _outer_indicator);

            
            SERIALIZE_FIELD(bool, _already_in_combat);

            SERIALIZE_FIELD(bool, _is_from_gautlet);

            SERIALIZE_FIELD(bool, _will_die);

            GameObject* _player = nullptr;
            GameObject* _audio_manager_game_object = nullptr;
            AudioManager* _audio_manager = nullptr;
            PlayerController* _player_controller = nullptr;
            PlayerCamera* _player_camera = nullptr;
            ComponentTransform* transform = nullptr;
            ComponentAnimation* animation = nullptr;
            CombatManager* _combat_manager = nullptr;
            ComponentAudioSource* _audio_source = nullptr;
            ComponentParticleSystem* _blood_trail_particles = nullptr; 
            ComponentParticleSystem* _small_dust_particles = nullptr;
            ComponentParticleSystem* _big_dust_particles = nullptr;
            ComponentBillboard* _inner_indicator_billboard = nullptr;
            ComponentBillboard* _outer_indicator_billboard = nullptr;

            Quat _spawn_rot;

            math::float3 _player_pos;
            math::float3 _target_pos;
            math::float3 _current_pos;
            float _attack_cooldown;

            SERIALIZE_FIELD(float, _attack_animation_duration);
            SERIALIZE_FIELD(float, _attack_animation_timer);

            BugState _state = BugState::SPAWNING;
            BugState _previous_state = BugState::INVALID;

            bool _parasite_dropped = false;
            bool _is_stunned = false;
            float _stun_time = 0.0f;
            float _acceleration = 0.0f;
            float _speed = 0.0f;
            float _current_idle_cooldown = 0.0f;
            float _enraged = 0.0f;
            float _enemy_dissolve_time = 1.0f;
            float _enemy_dissolving_time_progress = 0.0f;
            const float _enemy_dissolving = 1 / math::Sqrt(_enemy_dissolve_time);
            float _parasite_dissolve_time = 10.0f;
            float _parasite_dissolving_time_progress = 0.0f;
            const float _parasite_dissolving = 1 / math::Sqrt(_parasite_dissolve_time);
            float3 _knockback_pos = float3::zero;
            float _attack_current_delay = 0.f;
            SERIALIZE_FIELD(float, _current_spawning_time);
            //float _current_spawning_time = 0.f;
            bool _has_spawned = false;

            bool _inmune = false;
            bool _attack_landing = false;

            EnemyType _enemy_type = EnemyType::BEETLE;

            // Special attacks
            CombatManager::AttackStats push_attack;

            // Visual Effects:
            CombatVisualEffectsPool* _combat_visual_effects_pool;
        };
    } // namespace Scripting
} // namespace Hachiko*/
