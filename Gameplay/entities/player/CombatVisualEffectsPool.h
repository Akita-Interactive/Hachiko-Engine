#include <scripting/Script.h>
#include "entities/player/PlayerController.h"
#include <entities/enemies/EnemyController.h>

namespace Hachiko
{

	class GameObject;
	class ComponentBillboard;

	namespace Scripting
	{
		class CombatVisualEffectsPool : public Script
		{
			SERIALIZATION_METHODS(false);
		public:
			CombatVisualEffectsPool(GameObject* game_object);
			~CombatVisualEffectsPool() = default;

			void OnAwake() override;
			void OnUpdate() override {};

			void PlayPlayerAttackEffect(PlayerController::WeaponUsed weapon_type, int attack_index, float3 position);
			void PlayEnemyAttackEffect(EnemyType _enemy_type, float3 position);

			ComponentBillboard* GetCurrentClawVfx()
			{
				auto current = claw_vfxes[current_claw_vfx_index];
				current_claw_vfx_index = (current_claw_vfx_index + 1) % claw_vfxes.size();

				return current->GetComponent<ComponentBillboard>();
			}

			ComponentBillboard* GetCurrentSwordVfx()
			{
				auto current = sword_vfxes[current_sword_vfx_index];
				current_sword_vfx_index = (current_sword_vfx_index + 1) % sword_vfxes.size();

				return current->GetComponent<ComponentBillboard>();
			}

			ComponentBillboard* GetCurrentMeleeVfx()
			{
				auto current = melee_vfxes[current_melee_vfx_index];
				current_melee_vfx_index = (current_melee_vfx_index + 1) % melee_vfxes.size();

				return current->GetComponent<ComponentBillboard>();
			}

			ComponentBillboard* GetCurrentHammerVfx()
			{
				auto current = hammer_vfxes[current_hammer_vfx_index];
				current_hammer_vfx_index = (current_hammer_vfx_index + 1) % hammer_vfxes.size();

				return current->GetComponent<ComponentBillboard>();
			}

			ComponentBillboard* GetCurrentWormVfx()
			{
				auto current = worm_vfxes[current_worm_vfx_index];
				current_worm_vfx_index = (current_worm_vfx_index + 1) % worm_vfxes.size();

				return current->GetComponent<ComponentBillboard>();
			}

			ComponentBillboard* GetCurrentBeetleVfx()
			{
				auto current = beetle_vfxes[current_beetle_vfx_index];
				current_beetle_vfx_index = (current_beetle_vfx_index + 1) % beetle_vfxes.size();

				return current->GetComponent<ComponentBillboard>();
			}

		private:
			int current_claw_vfx_index = 0;
			std::vector<GameObject*> claw_vfxes;

			int current_sword_vfx_index = 0;
			std::vector<GameObject*> sword_vfxes;

			int current_melee_vfx_index = 0;
			std::vector<GameObject*> melee_vfxes;

			int current_hammer_vfx_index = 0;
			std::vector<GameObject*> hammer_vfxes;

			int current_worm_vfx_index = 0;
			std::vector<GameObject*> worm_vfxes;

			int current_beetle_vfx_index = 0;
			std::vector<GameObject*> beetle_vfxes;
		};
	} // namespace Scripting
} // namespace Hachiko