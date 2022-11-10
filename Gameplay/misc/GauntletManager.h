#pragma once

#include <scripting/Script.h>
#include "misc/DoorController.h"

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;

	namespace Scripting
	{
		class LevelManager;
		class CombatManager;
		class AudioManager;
		class PlayerCamera;

		class GauntletManager : public Script
		{
			SERIALIZATION_METHODS(false)
			
		public:
			GauntletManager(GameObject* game_object);
			~GauntletManager() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
			void ResetGauntlet(bool complete_reset);
			void StartGauntlet();
			bool IsCompleted() const { return completed; }
			bool IsFinished() const;
			bool IsStarted() const { return started; }

			SERIALIZE_FIELD(GameObject*, _combat_manager_go);
			SERIALIZE_FIELD(float, _trigger_radius);
			SERIALIZE_FIELD(float, _round_wait_time);
			SERIALIZE_FIELD(float, _complete_wait_time);

			SERIALIZE_FIELD(GameObject*, _door_controller_1);
			SERIALIZE_FIELD(GameObject*, _door_controller_2);
			SERIALIZE_FIELD(GameObject*, _closing_door);

			SERIALIZE_FIELD(GameObject*, _pack_1);
			SERIALIZE_FIELD(GameObject*, _pack_2);
			SERIALIZE_FIELD(GameObject*, _pack_3);		

			SERIALIZE_FIELD(GameObject*, _camera_anchor);
			SERIALIZE_FIELD(GameObject*, _central_anchor);
			SERIALIZE_FIELD(float3, _relative_position);
			SERIALIZE_FIELD(float, _camera_follows_player);

		private:
			
			void CheckRoundStatus();
			void OpenDoors();
			void CloseDoors();
			void SpawnRound(unsigned round);
			void ControllCameraPos();

			unsigned current_round = 0;
			bool completed = false;
			bool started = false;
			bool changing_rounds = false;
			float remaining_between_round_time{};
			CombatManager* _combat_manager = nullptr;
			LevelManager* _level_manager = nullptr;
			AudioManager* _audio_manager = nullptr;
			std::vector<GameObject*> _enemy_packs{};
			std::vector<DoorController*> _doors{};
			float _elapsed_time = 0.0f;
			// Camera change values
			PlayerCamera* _main_camera = nullptr;
		};
	}

}



