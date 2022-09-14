#pragma once

#include <scripting/Script.h>

#include "MathGeoLib.h"

namespace Hachiko
{
    class ComponentTransform;

    namespace Scripting
    {
        class TimeManager;
        class EnemyController;
        class PlayerController;
        class BossController;
        class CrystalExplosion;

        class CombatManager : public Script
        {
            SERIALIZATION_METHODS(false);

            struct BulletStats
            {
                friend CombatManager;
                float charge_time = 1.f;
                float lifetime = 3.f;
                float size = 3.0f;
                float speed = 50.f;
                float damage = 1.f;
                // Status effect
                float GetChargedPercent();
                bool update_ui = false;
            private:
                bool alive = false;
                bool shot = false;
                float elapsed_lifetime = 0.f;
                float current_charge = 0.f;
                float3 direction = float3::zero;
                float3 prev_position = float3::zero;
                ComponentTransform* emitter_transform = nullptr;
            };

            enum AttackType
            {
                CONE,
                RECTANGLE,
                CIRCLE
            };

            struct AttackStats
            {
                friend CombatManager;
                AttackType type = AttackType::RECTANGLE;
                unsigned damage = 1.f;
                float knockback_distance = 0.f;
                // Width is used as degrees for cone
                float width = 0.f;
                float range = 0.f;
                // Status effect
            };
            
        public:
            CombatManager(GameObject* game_object);
            ~CombatManager() override;
            
            void OnAwake() override;
            void OnUpdate() override;

            GameObject* GetPlayer() { return _player; }

            // Bool indicates if it hit something
            int PlayerMeleeAttack(const float4x4& origin, const AttackStats& attack_stats);
            int EnemyMeleeAttack(const float4x4& origin, const AttackStats& attack_stats);

            // If the emitter is deleted u are obligated to stop its bullet at that point to be safe
            // Player only system for now
            int ChargeBullet(ComponentTransform* emitter_transform, BulletStats new_stats);
            bool ShootBullet(unsigned bullet_index);
            void StopBullet(unsigned bullet_index);
            const BulletStats& GetBulletStats(unsigned bullet_idx) const;

            OBB CreateAttackHitbox(const float4x4& origin, const AttackStats& attack_stats) const;
            
            bool IsPackDead(GameObject* pack) const;
            bool IsPackDead(unsigned pack_idx) const;
            unsigned GetPackAliveCount(GameObject* pack) const;
            void ResetEnemyPack(GameObject* pack, bool is_gauntlet = false);
            void ActivateEnemyPack(GameObject* pack);
            void DeactivateEnemyPack(GameObject* pack);

        private:
            // Start attack depending on its type
            // TODO: Make this hit_enemies into a struct return type with int:
            int PlayerConeAttack(const float4x4& origin, const AttackStats& attack_stats, bool& hit_enemies);
            int PlayerRectangleAttack(const float4x4& origin, const AttackStats& attack_stats, bool& hit_enemies);
            int PlayerCircleAttack(const float4x4& origin, const AttackStats& attack_stats, bool& hit_enemies);

            int EnemyConeAttack(const float4x4& origin, const AttackStats& attack_stats);
            int EnemyRectangleAttack(const float4x4& origin, const AttackStats& attack_stats);
            int EnemyCircleAttack(const float4x4& origin, const AttackStats& attack_stats);

            // Evaluate for all level units
            int ProcessAgentsCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats, bool is_from_player);
            int ProcessObstaclesCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats);
            int ProcessPlayerCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats);
            int ProcessAgentsOBB(const OBB& attack_box, const AttackStats& attack_stats, const float3& attack_source_pos, bool is_from_player);
            int ProcessObstaclesOBB(const OBB& attack_box, const AttackStats& attack_stats);
            int ProcessPlayerOBB(const OBB& attack_box, const AttackStats& attack_stats, const float3& attack_source_pos);
            int ProcessAgentsCircle(const float3& attack_source_pos, const AttackStats& attack_stats, bool is_from_player);
            int ProcessObstaclesCircle(const float3& attack_source_pos, const AttackStats& attack_stats);
            int ProcessPlayerCircle(const float3& attack_source_pos, const AttackStats& attack_stats);
            // Evaluate for boss if exists Note: for now only added the attack the player uses
            int ProcessBossOBB(const OBB& attack_box, const AttackStats& attack_stats);

            // Specifics of the collision check
            bool ConeHitsAgent(GameObject* agent_go, const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance);
            bool ConeHitsObstacle(GameObject* obstacle_go, const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance);
            bool ConeHitsPlayer(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance);
            bool OBBHitsAgent(GameObject* agent_go, const OBB& attack_box);
            bool OBBHitsObstacle(GameObject* obstacle_go, const OBB& attack_box);
            bool OBBHitsPlayer(const OBB& attack_box);
            bool CircleHitsAgent(GameObject* agent_go, const float3& attack_source_pos, float radius);
            bool CircleHitsObstacle(GameObject* obstacle_go, const float3& attack_source_pos, float radius);
            bool CircleHitsPlayer(const float3& attack_source_pos, float radius);

            // What to do when system wants to register a hit
            void HitObstacle(GameObject* obstacle, float damage);
            void HitEnemy(EnemyController* enemy, int damage, float knockback = 0, float3 knockback_dir = float3::zero, bool is_from_player = false, bool is_ranged = false);
            void HitPlayer(int damage, float knockback = 0, float3 knockback_dir = float3::zero);

            // Bullet specific management operations
            void RunBulletSimulation();
            void UpdateBulletTrajectory(unsigned bullet_idx);
            bool CheckBulletCollisions(unsigned bullet_idx);
            void ActivateBullet(unsigned bullet_idx);
            void DeactivateBullet(unsigned bullet_idx);
            void SetBulletTrajectory(unsigned bullet_idx);

            // Processes hit and returns hit distance to check what to damage
            EnemyController* FindBulletClosestEnemyHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit);
            GameObject* FindBulletClosestObstacleHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit);

            // Enemy management
            void SerializeEnemyPacks();

        private:
            GameObject* _player = nullptr;
            GameObject* _boss = nullptr;
            unsigned _max_bullets = 5;
            std::vector<GameObject*> _bullets{};
            std::vector<BulletStats> _bullet_stats;

            GameObject* _enemy_packs_container;
            std::vector<std::vector<float4x4> > _initial_transforms;

            math::float3 _direction;
            int _damage;

            PlayerController* _player_controller;
            BossController* _boss_controller;
            TimeManager* _time_manager;

            SERIALIZE_FIELD(GameObject*, _charge_vfx);
            SERIALIZE_FIELD(GameObject*, _shot_vfx);

            ComponentParticleSystem* _charge_particles;
            ComponentParticleSystem* _shot_particles;
        };
    } // namespace Scripting
} // namespace Hachiko
