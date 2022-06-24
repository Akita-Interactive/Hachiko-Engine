#pragma once

#include <scripting/Script.h>

#include "MathGeoLib.h"

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;

    namespace Scripting
    {
        class EnemyController;
        class CrystalExplosion;

        class CombatManager : public Script
        {
            SERIALIZATION_METHODS(false);

            struct BulletStats
            {
                friend CombatManager;
                float charge_time = 1.f;
                float lifetime = 3.f;
                float size = 1.f;
                float speed = 50.f;
                float damage = 1.f;
                // Status effect
                float GetChargedPercent();
            private:
                bool alive = false;
                float elapsed_lifetime = 0.f;
                float current_charge = 0.f;
                float3 direction = float3::zero;
                float3 prev_position = float3::zero;
                ComponentTransform* emitter_transform = nullptr;
            };

            enum AttackType
            {
                CONE,
                RECTANGLE
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

            // Bool indicates if it hit something
            bool PlayerMeleeAttack(const float4x4& origin, const AttackStats& attack_stats);

            bool EnemyConeAttack(const float4x4& origin, float hit_angle_deg, float hit_distance, const AttackStats& attack_stats);
            bool EnemyRectangleAttack(const float4x4& origin, float width, float length, const AttackStats& attack_stats);

            // If the emitter is deleted u are obligated to stop its bullet at that point to be safe
            // Player only system for now
            int ShootBullet(ComponentTransform* emitter_transform, BulletStats new_stats);
            void StopBullet(unsigned bullet_index);
            const BulletStats& GetBulletStats(unsigned bullet_idx) const;

            OBB CreateAttackHitbox(const float4x4& origin, const AttackStats& attack_stats) const;

        private:
            // Start attack depending on its type
            bool PlayerConeAttack(const float4x4& origin, const AttackStats& attack_stats);
            bool PlayerRectangleAttack(const float4x4& origin, const AttackStats& attack_stats);

            // Evaluate for all units
            bool ProcessAgentsCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats);
            bool ProcessObstaclesCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats);
            bool ProcessAgentsOBB(const OBB& attack_box, const AttackStats& attack_stats);
            bool ProcessObstaclesOBB(const OBB& attack_box, const AttackStats& attack_stats);

            // Specifics of the collision check
            bool ConeHitsAgent(GameObject* agent_go, const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance);
            bool ConeHitsObstacle(GameObject* obstacle_go, const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance);
            bool OBBHitsAgent(GameObject* agent_go, const OBB& attack_box);
            bool OBBHitsObstacle(GameObject* obstacle_go, const OBB& attack_box);

            // What to do when system wants to register a hit
            void HitObstacle(CrystalExplosion* obstacle, float damage);
            void HitEnemy(EnemyController* enemy, float damage, float knockback = 0, float3 knockback_dir = float3::zero);

            // Bullet specific management operations
            void RunBulletSimulation();
            void UpdateBulletTrajectory(unsigned bullet_idx);
            bool CheckBulletCollisions(unsigned bullet_idx);
            void ActivateBullet(unsigned bullet_idx);
            void DeactivateBullet(unsigned bullet_idx);
            void SetBulletTrajectory(unsigned bullet_idx);

            // Processes hit and returns hit distance to check what to damage
            EnemyController* FindBulletClosestEnemyHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit);
            CrystalExplosion* FindBulletClosestObstacleHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit);
        
        private:
            unsigned _max_bullets = 5;
            std::vector<GameObject*> _bullets{};
            std::vector<BulletStats> _bullet_stats;

            math::float3 _direction;
            int _damage;
        };
    } // namespace Scripting
} // namespace Hachiko
