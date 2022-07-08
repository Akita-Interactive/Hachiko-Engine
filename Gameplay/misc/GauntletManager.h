#pragma once

#include <scripting/Script.h>
#include "entities/player/CombatManager.h"
#include "misc/DoorController.h"

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;
	namespace Scripting
	{
		class GauntletManager : public Script
		{
			SERIALIZATION_METHODS(false)
			
		public:
			GauntletManager(GameObject* game_object);
			~GauntletManager() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
			void ResetGauntlet();
			bool IsCompleted() const {return completed;}
			bool IsFinished() const;

			SERIALIZE_FIELD(GameObject*, _combat_manager_go);
			SERIALIZE_FIELD(float, _trigger_radius);
			SERIALIZE_FIELD(float, _round_wait_time);
			SERIALIZE_FIELD(float, _complete_wait_time);

			SERIALIZE_FIELD(GameObject*, _door_controller_1);
			SERIALIZE_FIELD(GameObject*, _door_controller_2);

			SERIALIZE_FIELD(GameObject*, _pack_1);
			SERIALIZE_FIELD(GameObject*, _pack_2);
			SERIALIZE_FIELD(GameObject*, _pack_3);		

		private:

			void StartGauntlet();
			void CheckRoundStatus();
			void OpenDoors();
			void CloseDoors();
			void SpawnRound(unsigned round);

			unsigned current_round = 0;
			bool completed = false;
			bool started = false;
			bool changing_rounds = false;
			float remaining_between_round_time;
			CombatManager* _combat_manager = nullptr;
			std::vector<GameObject*> _enemy_packs{};
			std::vector<DoorController*> _doors{};

		};
	}

}



