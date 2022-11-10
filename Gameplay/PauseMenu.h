#pragma once

#include "scriptingUtil/gameplaypch.h"
#include <scripting/Script.h>
#include "misc/LevelManager.h"
#include "constants/Scenes.h"

namespace Hachiko
{
	class GameObject;
	class ComponentButton;
	namespace Scripting
	{
		class PauseMenu : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			PauseMenu(GameObject* game_object);
			~PauseMenu() override = default;

			void OnAwake() override;
			void OnUpdate() override;
			
		private:
			void HandleButtonInteractions();

			SERIALIZE_FIELD(GameObject*, _pause_ui);
			SERIALIZE_FIELD(GameObject*, _player_hud);
			LevelManager* _level_manager;

			SERIALIZE_FIELD(ComponentButton*, _resume_button);
			SERIALIZE_FIELD(ComponentButton*, _quit_button);

			bool _game_paused;
		};
	}
}

