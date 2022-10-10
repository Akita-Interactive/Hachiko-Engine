#include "scriptingUtil/gameplaypch.h"
#include "PauseMenu.h"

Hachiko::Scripting::PauseMenu::PauseMenu(GameObject* game_object)
	: Script(game_object, "PauseMenu")
{}

void Hachiko::Scripting::PauseMenu::OnAwake()
{
	_level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
}

void Hachiko::Scripting::PauseMenu::OnUpdate()
{
	if (!_player_hud || !_pause_ui) return;
	if (Input::IsKeyDown(Input::KeyCode::KEY_ESCAPE) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_START))
	{
		_game_paused = !_game_paused;
		if (_game_paused)
		{
			_player_hud->SetActive(false);
			_pause_ui->SetActive(true);
			_level_manager->BlockInputs(true);
			Time::SetTimeScale(0.0f);
		}
		else
		{
			_player_hud->SetActive(true);
			_pause_ui->SetActive(false);
			_level_manager->BlockInputs(false);
			Time::SetTimeScale(1.0f);
		}
	}

	if (_game_paused)
	{
		HandleButtonInteractions();
	}
}

void Hachiko::Scripting::PauseMenu::HandleButtonInteractions()
{
	if (_resume_button->IsSelected())
	{
		_game_paused = false;
		_player_hud->SetActive(true);
		_pause_ui->SetActive(false);
		_level_manager->BlockInputs(false);
		Time::SetTimeScale(1.0f);
	}

	if (_quit_button->IsSelected())
	{
		_level_manager->BlockInputs(false);
		Time::SetTimeScale(1.0f);
		SceneManagement::SwitchScene(Scenes::MAIN_MENU);
	}
}