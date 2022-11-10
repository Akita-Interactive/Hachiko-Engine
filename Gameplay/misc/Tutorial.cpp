#include "scriptingUtil/gameplaypch.h"

#include "misc/TimeManager.h"
#include "constants/Scenes.h"
#include "constants/Sounds.h"
#include "entities/player/PlayerController.h"
#include "entities/enemies/EnemyController.h"
#include "misc/LevelManager.h"

#include "Tutorial.h"

Hachiko::Scripting::Tutorial::Tutorial(GameObject* game_object)
	: Script(game_object, "Tutorial")
	, _tutorial_keyboard_movement(nullptr)
	, _tutorial_controller_movement(nullptr)
	, _tutorial_keyboard_attacks(nullptr)
	, _tutorial_controller_attacks(nullptr)
	, _tutorial_keyboard_pickup(nullptr)
	, _tutorial_controller_pickup(nullptr)
	, _first_enemy(nullptr)
	, _tutorials_screen_shown(0)
{
}

void Hachiko::Scripting::Tutorial::OnAwake()
{
	_level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();

	_time_manager = game_object->GetComponent<TimeManager>();

	_player = Scenes::GetPlayer();
	if (_player)
	{
		_player_controller = _player->GetComponent<PlayerController>();
	}

	if (_first_enemy)
	{
		_enemy_controller = _first_enemy->GetComponent<EnemyController>();
	}

	_audio_source = game_object->GetComponent<ComponentAudioSource>();

	_tutorials_screen_shown = 0;
}

void Hachiko::Scripting::Tutorial::OnUpdate()
{
	if (_tutorials_screen_shown == 3)
	{
		return;
	}
	else if (_tutorials_screen_shown == 0)
	{
		SetMovementScreen(true);
		if (TutorialButtonPressed())
		{
			SetMovementScreen(false);
			_tutorials_screen_shown = 1;
			return;
		}
	}
	else if (_tutorials_screen_shown == 1)
	{
		ShowAttackScreen(true);
		if (TutorialButtonPressed())
		{
			ShowAttackScreen(false);
			// After finishing firs tutorial block restart the game
			_time_manager->InterpolateToTimeScale(1.0f, 0.1f, 0.2f);
			_tutorials_screen_shown = 2;
			return;
		}
	}
	
	else if (_tutorials_screen_shown == 2 && _enemy_controller->ParasiteDropped())
	{
		ShowParasyteScreen(true);
		Time::SetTimeScale(0.0001f);
		if (TutorialButtonPressed())
		{
			ShowParasyteScreen(false);
			_time_manager->InterpolateToTimeScale(1.0f, 0.1f, 0.2f);
			_tutorials_screen_shown = 3;
			return;
		}
	}

	first_active_frame = false;
}


void Hachiko::Scripting::Tutorial::SetMovementScreen(bool show)
{
	ShowTutorialScreen(show, _tutorial_keyboard_movement, _tutorial_controller_movement);
}

void Hachiko::Scripting::Tutorial::ShowAttackScreen(bool show)
{
	ShowTutorialScreen(show, _tutorial_keyboard_attacks, _tutorial_controller_attacks);
}

void Hachiko::Scripting::Tutorial::ShowParasyteScreen(bool show)
{
	ShowTutorialScreen(show, _tutorial_keyboard_pickup, _tutorial_controller_pickup);
}

void Hachiko::Scripting::Tutorial::ShowTutorialScreen(bool show, GameObject* keyboard_screen, GameObject* controller_screen)
{
	// Trigger audio events only when show changes
	if (show != showing_screen)
	{
		if (show)
		{
			_audio_source->PostEvent(Sounds::HOVER);

		}
		else
		{
			_audio_source->PostEvent(Sounds::CLICK);

		}
		showing_screen = show;
	}

	_level_manager->BlockInputs(show);
	if (!show)
	{
		keyboard_screen->SetActive(false);
		controller_screen->SetActive(false);
		return;
	}
	if (Input::IsGamepadModeOn())
	{
		controller_screen->SetActive(true);
		return;
	}
	keyboard_screen->SetActive(true);
}

bool Hachiko::Scripting::Tutorial::TutorialButtonPressed()
{
	if (first_active_frame)
	{
		return false;
	}
	return Input::IsKeyDown(Input::KeyCode::KEY_Y) ||
		Input::IsKeyDown(Input::KeyCode::KEY_F) ||
		Input::IsKeyDown(Input::KeyCode::KEY_SPACE) ||
		Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_Y) ||
		Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_A) ||
		Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_B);
}
