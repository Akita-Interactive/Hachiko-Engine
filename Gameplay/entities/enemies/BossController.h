#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"
#include "entities/player/CombatManager.h"
#include "misc/GauntletManager.h"

#include <queue>

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;
    class ComponentParticleSystem;
    class ComponentBillboard;
    class ComponentProgressBar;

    namespace Scripting
    {
        class AudioManager;
        class PlayerController;
        class CombatVisualEffectsPool;

        class PlayerCamera;

        enum class BossState
        {
            WAITING_ENCOUNTER,
            STARTING_ENCOUNTER,
            COMBAT_FORM,
            CACOON_FORM,
            DEAD,
        };

        // States while in BossState::COMBAT_FORM
        enum class CombatState
        {
            IDLE,
            CHASING,
            ATTACKING,
            SPAWNING_CRYSTALS,
            CONSUMING_PARASYTES,
            CRYSTAL_JUMP,
        };

        class BossController : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            BossController(GameObject* game_object);
            ~BossController() override = default;

            void OnAwake() override;
            void OnStart() override;
            void OnUpdate() override;
            
            BossState GetState() const { return state; };
            bool IsAlive() const;
            void RegisterHit(int dmg);
            void UpdateHpBar();


        private:
            void SetUpHpBar();
            void SetHpBarActive(bool v);
            void StateController();
            void StateTransitionController();
            void CombatController();
            void CombatTransitionController();

            float GetPlayerDistance();

            // Fight States
            void WaitingController();
            void StartEncounter();
            void StartEncounterController();
            void ResetCombatState();
            void Die();
            void DieController();

            // Cacoon
            void StartCacoon();
            void CacoonController();

            bool CacoonTrigger();
            void FinishCacoon();

            // Combat States (While COMBAT_FORM)
            void Chase();
            void ChaseController();

            void MeleeAttack();
            void MeleeAttackController();

            void SpawnCrystals();
            void SpawnCrystalsController();
            void ConsumeParasytes();
            void ConsumeParasytesController();

        private:
            SERIALIZE_FIELD(int, state_value);
            SERIALIZE_FIELD(GameObject*, hp_bar_go);
            SERIALIZE_FIELD(GameObject*, crystal_target_go);
            SERIALIZE_FIELD(GameObject*, cocoon_placeholder_go);
            SERIALIZE_FIELD(GameObject*, gauntlet_go);
            SERIALIZE_FIELD(float, start_encounter_range);
            GameObject* player = nullptr; // It's found on scene based on name
            ComponentTransform* transform = nullptr;
            ComponentProgressBar* hp_bar = nullptr;
            ComponentAgent* agent = nullptr;
            GauntletManager* gauntlet = nullptr;
            Stats* combat_stats = nullptr;
            BossState state = BossState::WAITING_ENCOUNTER;
            BossState prev_state = state;
            CombatState combat_state = CombatState::IDLE;
            CombatState prev_combat_state = combat_state;
            bool hitable = true;
            std::vector<float> gauntlet_thresholds_percent{0.3, 0.7};
            float3 target_position = float3::zero;
        };
    } // namespace Scripting
} // namespace Hachiko*/
