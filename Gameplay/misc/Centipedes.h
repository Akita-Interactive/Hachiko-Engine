#pragma once

#include "constants/Sounds.h"
#include <scripting/Script.h>

namespace Hachiko
{
	namespace Scripting
	{
		class AudioManager;

		class Centipedes : public Script
		{
			SERIALIZATION_METHODS(false)

				enum class CentipedeState {
				IDLE,
				RUN,
				DIE
			};

		public:
			Centipedes(GameObject* game_object);
			~Centipedes() override = default;

			void OnAwake() override;
			void OnUpdate() override;

			CentipedeState GetState() {
				return _state;
			}

		private:

			void UpdateCentipedeState();
			void SteppedOn();

			CentipedeState _state = CentipedeState::IDLE;
			CentipedeState _previous_state = CentipedeState::IDLE;

			float player_range = 10.0f;
			float stepped_on_range = 1.0f;
			float rotation_smoothness = 0.2f;

			float centipide_aggro = 40.0f;

			GameObject* _player = nullptr;

			ComponentTransform* player_transform = nullptr;
			ComponentAgent* agent = nullptr;
			ComponentAnimation* animation = nullptr;
			ComponentAudioSource* audio_source = nullptr;

			SERIALIZE_FIELD(GameObject*, _damage_effect);
			ComponentBillboard* _splash_effect_billboard = nullptr;

			float time_to_disapear = 20.0f;
			float before_disapear = 0.0f;
		};
	}
}


