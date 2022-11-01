#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"

namespace Hachiko
{
	namespace Scripting
	{
		enum class StalagmiteState
		{
			INVALID,
			FALLING,
			COLLAPSED,
			DISSOLVING
		};

		class Stalagmite : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			Stalagmite(GameObject* game_object);
			~Stalagmite() override = default;

			StalagmiteState GetState() 
			{
				return stalagmite_state;
			}

			StalagmiteState GetPreviousState() 
			{
				return _previous_state;
			}

			void SetNewState(StalagmiteState state)
			{
				stalagmite_state = state;
			}

			void SetPreviousState(StalagmiteState state)
			{
				_previous_state = state;
			}

			GameObject* GetGEO() 
			{
				return GEO;
			}

			GameObject* GetExplosionArea()
			{
				return _explosion_effect;
			}

			void ActiveStalagmite();
			void ActiveEffects();
			void Falling(float fall_progress, const GameObject* player);

			bool IsStalagmiteCollapsed() 
			{
				return _stalagmite_collapsed;
			}

			void SetStalagmiteOnGround()
			{
				_stalagmite_collapsed = true;
			}

			void Dissolved();

			float _dissolving_time = 0.0f;

		private:

			StalagmiteState stalagmite_state = StalagmiteState::INVALID;
			StalagmiteState _previous_state = StalagmiteState::INVALID;

			SERIALIZE_FIELD(GameObject*, _explosion_effect);
			SERIALIZE_FIELD(GameObject*, _area_indicator);
			SERIALIZE_FIELD(GameObject*, GEO);
			SERIALIZE_FIELD(GameObject*, _obstacle);

			ComponentObstacle* _obstacle_comp = nullptr;
			ComponentAudioSource* _audio_source = nullptr;

			bool _stalagmite_collapsed = false;
		};
	}
}


