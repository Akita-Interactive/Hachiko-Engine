#include "scriptingUtil/gameplaypch.h"

#include "LevelManager.h"

#include "entities/player/PlayerController.h"

#include "Gameplay.h"
#include "constants/Scenes.h"

Hachiko::Scripting::LevelManager::LevelManager(GameObject* game_object)
	: Script(game_object, "LevelManager")
	, _level(1)
	, _respawn_position(float3::zero)
	, _last_gauntlet(nullptr)
{}

void Hachiko::Scripting::LevelManager::OnAwake()
{
	_enemy_counter = _gauntlet_counter_go->GetComponent<ComponentText>();
	_gauntlet_ui_go->SetActive(false);
}

void Hachiko::Scripting::LevelManager::OnUpdate()
{
	if (!_last_gauntlet) return;
	_gauntlet_ui_go->SetActive(_last_gauntlet && !_last_gauntlet->IsCompleted());

	if (!_player_hud || !_pause_menu) return;
	if (Input::IsKeyPressed(Input::KeyCode::KEY_ESCAPE) || Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_START)) {
		_game_paused = !_game_paused;
		if (_game_paused)
		{
			_player_hud->SetActive(false);
			Time::SetTimeScale(0.0f);
		}
		else
		{
			_player_hud->SetActive(true);
			Time::SetTimeScale(1.0f);
		}
	}
}	

void Hachiko::Scripting::LevelManager::SetGauntlet(GauntletManager* last_gauntlet)
{
	_last_gauntlet = last_gauntlet;
}

void Hachiko::Scripting::LevelManager::SetEnemyCount(unsigned count)
{
	_enemy_counter->SetText(std::to_string(count).c_str());
}

float3 Hachiko::Scripting::LevelManager::Respawn()
{
	if (_last_gauntlet != nullptr && !_last_gauntlet->IsCompleted())
	{
		_last_gauntlet->ResetGauntlet();
	}

	return GetRespawnPosition();

	//Disable gauntlet ui
}

void Hachiko::Scripting::LevelManager::GoalReached() 
{
	if (_level == 1)
	{
		SceneManagement::SwitchScene(Scenes::LEVEL2);
	}
	else if (_level == 2)
	{
		SceneManagement::SwitchScene(Scenes::BOSS_LEVEL);
	}
	else 
	{
		SceneManagement::SwitchScene(Scenes::MAIN_MENU);
	}
}