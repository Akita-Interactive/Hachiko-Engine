#pragma once

#include <scripting/Script.h>
#include "misc/GauntletManager.h"

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;
	class ComponentText;
	namespace Scripting
	{
		class PlayerController;
		
		class LevelManager : public Script
		{
			SERIALIZATION_METHODS(false)
			
		public:
			LevelManager(GameObject* game_object);
			~LevelManager() override = default;

			void OnAwake() override;
			void OnUpdate() override;

			void SetGauntlet(GauntletManager* last_gauntlet);
			void SetEnemyCount(unsigned count);

			void SetRespawnPosition(const float3& new_respawn_position) 
			{
				_respawn_position = new_respawn_position;
			}

			[[nodiscard]] float3 GetRespawnPosition() const
			{
				return _respawn_position;
			}

			float3 Respawn();
			void GoalReached();

			void BlockInputs(bool block)
			{
				_inputs_blocked = block;
			}
			[[nodiscard]] bool AreInputsBlocked() const
			{
				return _inputs_blocked;
			}

			SERIALIZE_FIELD(unsigned, _level);
			SERIALIZE_FIELD(float3, _respawn_position);
			SERIALIZE_FIELD(GameObject*, _gauntlet_ui_go);
			SERIALIZE_FIELD(GameObject*, _gauntlet_counter_go);

			SERIALIZE_FIELD(GameObject*, _pause_menu);
			SERIALIZE_FIELD(GameObject*, _player_hud);

		private:
			GauntletManager* _last_gauntlet = nullptr;
			ComponentText* _enemy_counter = nullptr;
			bool _inputs_blocked = false;
			bool _game_paused = false;
		};
	}

}



