#pragma once
#include <scripting/Script.h>
#include "entities/enemies/EnemyController.h"

namespace Hachiko
{
	namespace Scripting
	{
		class EnemyController;

		class BugAnimationManager : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			BugAnimationManager(Hachiko::GameObject* game_object);
			~BugAnimationManager() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
		
		private:
			SERIALIZE_FIELD(ComponentAnimation*, _animator);
			SERIALIZE_FIELD(BugState, _previous_state);
			SERIALIZE_FIELD(std::string, _state_string);
			SERIALIZE_FIELD(unsigned, _idle_index);
			SERIALIZE_FIELD(unsigned, _attacking_index);

			EnemyController* _enemy_controller;
		};
	}
}