#pragma once

#include <scripting/Script.h>
#include "Gameplay.h"

namespace Hachiko
{
	class GameObject;
	namespace Scripting
	{
		class TimeManager;
		class PlayerController;
		class EnemyController;
		class LevelManager;

		class Tutorial : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			Tutorial(GameObject* game_object);
			~Tutorial() override = default;

			void OnAwake() override;
			void OnUpdate() override;

		private:
			void SetMovementScreen(bool show);
			void ShowAttackScreen(bool show);
			void ShowParasyteScreen(bool show);

			void ShowTutorialScreen(bool show, GameObject* keyboard_screen, GameObject* controller_screen);
			bool TutorialButtonPressed();

		private:
			SERIALIZE_FIELD(GameObject*, _tutorial_keyboard_movement);
			SERIALIZE_FIELD(GameObject*, _tutorial_controller_movement);
			SERIALIZE_FIELD(GameObject*, _tutorial_keyboard_attacks);
			SERIALIZE_FIELD(GameObject*, _tutorial_controller_attacks);
			SERIALIZE_FIELD(GameObject*, _tutorial_keyboard_pickup);
			SERIALIZE_FIELD(GameObject*, _tutorial_controller_pickup);
			SERIALIZE_FIELD(GameObject*, _first_enemy);

			TimeManager* _time_manager;
			GameObject* _player = nullptr;
			GameObject* _player_tutorial = nullptr;
			GameObject* _parasite_tutorial = nullptr;
			PlayerController* _player_controller;
			EnemyController* _enemy_controller;
			LevelManager* _level_manager;
			ComponentAudioSource* _audio_source = nullptr;

			SERIALIZE_FIELD(int, _tutorials_screen_shown);
			bool showing_screen = false;
			bool first_active_frame = true;
		};
	} // namespace Scripting
} // namespace Hachiko
