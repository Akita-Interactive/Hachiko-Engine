#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
	class GameObject;

	namespace Scripting
	{
		class CombatManager;

		class Spawner : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			Spawner(GameObject* game_object);
			~Spawner() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
		private:
			SERIALIZE_FIELD(GameObject*, _combat_manager);
			CombatManager* _combat_manager_script = nullptr;
			SERIALIZE_FIELD(GameObject*, _enemy_pack);
			SERIALIZE_FIELD(GameObject*, _player);
		};
	}
}