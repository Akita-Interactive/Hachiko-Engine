#pragma once

#include <scripting/Script.h>
#include <entities/crystals/Stalagmite.h>

namespace Hachiko
{
	namespace Scripting
	{

		class PlayerCamera;

		class StalagmiteManager : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			StalagmiteManager(GameObject* game_object);
			~StalagmiteManager() override = default;
	
			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;

			void UpdateStalagmiteState(Stalagmite* stalagmite);

			void FallingStalagmite(Stalagmite* stalagmite);

			void TriggerStalagmites();

			bool CheckPreviousStalagmite(int idx);

		private:
			void GenerateStalagmites();

		private:

			float falling_elapsed = 0.0f;
			float cooldown_elapsed = 0.0f;
			float _falling_time = 0.5f;

			bool _should_fall_stalagmites = false;

			PlayerCamera* _player_camera = nullptr;
			PlayerController* _player_controller = nullptr;
			CombatManager* _combat_manager = nullptr;

			GameObject* _player = nullptr;
			math::float3 _player_pos;

			SERIALIZE_FIELD(float, _falling_cooldown);

			// STALAGMITE
			std::vector<Stalagmite*> _stalagmites{};

		};
	}
}

