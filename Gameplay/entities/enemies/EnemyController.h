#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"
#include "entities/player/CombatManager.h"
#include <functional>

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
            WORM,
        };

        enum class EnemyState
        {
            INVALID,
            DEAD,
            IDLE,
            ATTACKING,
            PARASITE,
            SPAWNING,
            SUPER_DEAD,
            // Beetle unique states
            MOVING,
            MOVING_BACK,
            PATROL,
            // Worm unique states
            HIDEN,
            HIT,
            COUNT
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
            
            EnemyState GetState() const { return _state; };

            const Stats* GetStats() { return _combat_stats; };

            bool IsAlive() { return _combat_stats->IsAlive(); };
            void SetIsFromGauntlet(bool v) { _is_from_gautlet = v; }
            void SetIsFromBoss(bool v) { _is_from_boss = v; }
            void RegisterHit(int player_atk, math::float3 direction, float knockback, bool is_from_player, bool is_ranged);
            void GetParasite();
            bool ParasiteDropped() { return _parasite_dropped; };
            // Instantly kill an enemy regardless of its state
            void SetDead() { _force_state = EnemyState::DEAD; }

            void ResetEnemy();
            void ResetEnemyPosition();

        private:
            void GetComponents();
            void SetStats();
            void SetVfx();
            void SetUpWormVfx();
            
            void RecieveKnockback();

            float4x4 GetMeleeAttackOrigin(float attack_range) const;
            void StopMoving();
            void MoveInNavmesh(const float3& target_pos);
            
            void DestroyEntity();

            bool IsAttacking() const { return _state == EnemyState::ATTACKING; }

            // The worm attack is completely separated from the worm itself once its shot
            void SpitController();

            bool CheckValidPath();
            bool CheckValidPath(float3 position);

        private:
            struct StateBehaviour {
                std::function<void()> Start = nullptr;
                std::function<void()> Update = nullptr;
                std::function<void()> End = nullptr;
                std::function<EnemyState()> Transitions = nullptr;

                StateBehaviour() {};

                StateBehaviour(std::function<void()> start, std::function<void()> update, std::function<void()> end, std::function<EnemyState()> transitions)
                {
                    Start = start;
                    Update = update;
                    End = end;
                    Transitions = transitions;
                }
            };
            StateBehaviour states_behaviour[static_cast<int>(EnemyState::COUNT)];

            void BeetleUpdate();
            void WormUpdate();

            // SPAWNING
            void StartSpawningState();
            void UpdateSpawningState();
            void EndSpawningState();
            EnemyState TransitionsSpawningState();
            // IDLE
            void StartIdleState();
            void UpdateIdleState();
            void EndIdleState();
            EnemyState TransitionsIdleState();
            // ATTACKING
            void StartAttackingState();
            void UpdateAttackingState();
            void EndAttackingState();
            EnemyState TransitionsAttackingState();
            // MOVING
            void StartMovingState();
            void UpdateMovingState();
            void EndMovingState();
            EnemyState TransitionsMovingState();
            // MOVING_BACK
            void StartMovingBackState();
            void UpdateMovingBackState();
            void EndMovingBackState();
            EnemyState TransitionsMovingBackState();
            // PATROL
            void StartPatrolState();
            void UpdatePatrolState();
            void EndPatrolState();
            EnemyState TransitionsPatrolState();
            // HIT
            void StartHitState();
            void UpdateHitState();
            void EndHitState();
            EnemyState TransitionsHitState();
            // DEAD
            void StartDeadState();
            void UpdateDeadState();
            void EndDeadState();
            EnemyState TransitionsDeadState();
            // PARASITE
            void StartParasiteState();
            void UpdateParasiteState();
            void EndParasiteState();
            EnemyState TransitionsParasiteState();

            /* WORM */
            // SPAWNING
            void WormStartSpawningState();
            void WormUpdateSpawningState();
            void WormEndSpawningState();
            EnemyState WormTransitionsSpawningState();
            // IDLE
            void WormStartIdleState();
            void WormUpdateIdleState();
            void WormEndIdleState();
            EnemyState WormTransitionsIdleState();
            // ATTACKING
            void WormStartAttackingState();
            void WormUpdateAttackingState();
            void WormEndAttackingState();
            EnemyState WormTransitionsAttackingState();
            // HIT
            void WormStartHitState();
            void WormUpdateHitState();
            void WormEndHitState();
            EnemyState WormTransitionsHitState();
            // HIDEN
            void WormStartHidenState();
            void WormUpdateHidenState();
            void WormEndHidenState();
            EnemyState WormTransitionsHidenState();

            bool forced_state = false;
            EnemyState _force_state = EnemyState::INVALID;

            Stats* _combat_stats;
            SERIALIZE_FIELD(int, _state_num);
            SERIALIZE_FIELD(bool, _worm);
            SERIALIZE_FIELD(int, _aggro_range);
            SERIALIZE_FIELD(float, _attack_range);
            SERIALIZE_FIELD(float, _attack_delay);
            SERIALIZE_FIELD(float, _idle_cooldown);
            SERIALIZE_FIELD(float, _patrol_cooldown);
            SERIALIZE_FIELD(float, _spawning_time);
            SERIALIZE_FIELD(float, _chase_cooldown);
            float _chase_remaining_cooldown;
            SERIALIZE_FIELD(float3, _spawn_pos);
            
            SERIALIZE_FIELD(GameObject*, _enemy_body);
            SERIALIZE_FIELD(GameObject*, _parasite);
            GameObject* _blood_trail = nullptr;
            SERIALIZE_FIELD(GameObject*, _small_dust);
            SERIALIZE_FIELD(GameObject*, _big_dust);
            SERIALIZE_FIELD(GameObject*, _attack_zone);
            SERIALIZE_FIELD(GameObject*, _inner_indicator);
            SERIALIZE_FIELD(GameObject*, _outer_indicator);
            SERIALIZE_FIELD(GameObject*, _projectile_particles);
            SERIALIZE_FIELD(GameObject*, _explosion_particles);

            
            SERIALIZE_FIELD(bool, _already_in_combat);

            SERIALIZE_FIELD(bool, _is_from_gautlet);
            SERIALIZE_FIELD(bool, _is_from_boss);

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

        	ComponentBillboard* _blood_trail_billboard = nullptr;
            ComponentParticleSystem* _small_dust_particles = nullptr;
            ComponentParticleSystem* _big_dust_particles = nullptr;
            ComponentBillboard* _inner_indicator_billboard = nullptr;
            ComponentBillboard* _outer_indicator_billboard = nullptr;
            ComponentParticleSystem* _projectile_particles_comp = nullptr;
            ComponentParticleSystem* _explosion_particles_comp = nullptr;

        	ComponentAgent* _component_agent = nullptr;

            Quat _spawn_rot;

            math::float3 _player_pos;
            math::float3 _target_pos;
            math::float3 _current_pos;
            float _attack_cooldown;

            SERIALIZE_FIELD(float, _attack_animation_duration);
            SERIALIZE_FIELD(float, _attack_animation_timer);

            EnemyState _state = EnemyState::SPAWNING;
            EnemyState _previous_state = EnemyState::INVALID;

            // Enemy status
            bool _parasite_dropped = false;
            bool _is_stunned = false;
            // Enemy exclusive stats
            float _stun_time = 0.0f;
            float _acceleration = 0.0f;
            float _speed = 0.0f;
            float _current_idle_cooldown = 0.0f;
            float _current_patrol_cooldown = 0.0f;
            float _enraged = 0.0f;
            float _enemy_dissolve_time = 1.0f;
            float _enemy_dissolving_time_progress = 0.0f;
            const float _enemy_dissolving = 1 / math::Sqrt(_enemy_dissolve_time);
            // Parasite exclusive stats
            float _parasite_dissolve_time = 10.0f;
            float _parasite_dissolving_time_progress = 0.0f;
            const float _parasite_dissolving = 1 / math::Sqrt(_parasite_dissolve_time);
            // Combat info
            float3 _knockback_pos = float3::zero;
            float _attack_current_delay = 0.f;
            // Spawning times
            float _current_spawning_time = 2.0f;
            float spawn_progress = 0.0f;
            const float spawn_rate = 0.5f;
            bool _has_spawned = false;

            bool _immune = false;
            bool _attack_landing = false;
            bool _spit_shot = false;
            bool _attack_alt = false;

            SERIALIZE_FIELD(float, damage_effect_duration);
            float damage_effect_progress = 0.0f;

            bool _valid_path = false;
            float _timer_check_path = 0.0f;

            EnemyType _enemy_type = EnemyType::BEETLE;

            // Special attacks
            CombatManager::AttackStats push_attack;

            // Visual Effects:
            CombatVisualEffectsPool* _combat_visual_effects_pool;
        };
    } // namespace Scripting
} // namespace Hachiko*/
