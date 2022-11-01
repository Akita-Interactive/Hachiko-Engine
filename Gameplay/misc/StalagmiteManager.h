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

			void FallingStalagmite(Stalagmite* stalagmite, float fall_progress);

			void TriggerStalagmites();

			bool CheckPreviousStalagmite(int idx);

			void DestroyAllStalagmites(bool force = false);

			bool AllStalactitesCollapsed();

		private:
			void GenerateStalagmites();

			void KnockbackOnEnemies(float3 position);

		private:

			float falling_elapsed = -1.f;
			float cooldown_elapsed = 0.0f;
			float _falling_time = 0.5f;
			float _total_dissolving_time = 2.5f;

			bool _should_fall_stalagmites = false;
			bool _should_update_stalagmites = false;

			PlayerCamera* _player_camera = nullptr;
			PlayerController* _player_controller = nullptr;
			CombatManager* _combat_manager = nullptr;

			GameObject* _player = nullptr;
			math::float3 _player_pos;
			//SERIALIZE_FIELD(bool, _should_fall_stalagmites);
			//SERIALIZE_FIELD(bool, _should_update_stalagmites);
			SERIALIZE_FIELD(float, _falling_cooldown);


			float _stalactites_life = 35.0f;
			float _stalactites_timer = 0.0f;

			// STALAGMITE
			std::vector<Stalagmite*> _stalagmites{};

		};
	}
}

