#pragma once

#include <scripting/Script.h>
#include "misc/GauntletManager.h"

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;
	namespace Scripting
	{
		class PlayerController;
		class LevelManager : public Script
		{
			SERIALIZATION_METHODS(false)
			
		public:
			LevelManager(GameObject* game_object);
			~LevelManager() override = default;

			void SetLastGauntlet(GauntletManager* last_gauntlet) 
			{
				_last_gauntlet = last_gauntlet;
			}

			void SetRespawnPosition(const float3& new_respawn_position) 
			{
				_respawn_position = new_respawn_position;
			}

			[[nodiscard]] float3 GetRespawnPosition() const
			{
				return _respawn_position;
			}

			void Respawn(PlayerController* player);

			SERIALIZE_FIELD(float3, _respawn_position);


		private:

			GauntletManager* _last_gauntlet = nullptr;

		};
	}

}



