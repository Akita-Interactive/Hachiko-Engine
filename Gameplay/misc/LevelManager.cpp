#include "scriptingUtil/gameplaypch.h"

#include "LevelManager.h"

#include "entities/player/PlayerController.h"

#include "Gameplay.h"
#include "constants/Scenes.h"
#include "AudioManager.h"

Hachiko::Scripting::LevelManager::LevelManager(GameObject* game_object)
	: Script(game_object, "LevelManager")
	, _level(1)
	, _respawn_position(float3::zero)
	, _last_gauntlet(nullptr)
	, _modify_fog(false)
	, _fog_frequency(0.1)
	, _fog_max_density(0.015)
	, _fog_min_density(0.005)
{}

void Hachiko::Scripting::LevelManager::OnAwake()
{
	_enemy_counter = _gauntlet_counter_go->GetComponent<ComponentText>();
	if (_audio_manager_go != nullptr)
	{
		_audio_manager = _audio_manager_go->GetComponent<AudioManager>();
		_audio_manager->SetLevel(_level);
	}

	_gauntlet_ui_go->SetActive(false);

	_time = 0;
}

void Hachiko::Scripting::LevelManager::OnUpdate()
{
	if (_modify_fog)
	{
		_time += Time::DeltaTime();

		float avg_density = (_fog_min_density + _fog_max_density) / 2;
		float amp_density = (_fog_max_density - _fog_min_density) / 2;
		SceneManagement::SetFogGlobalDensity(avg_density + amp_density * (math::Sin(_time * _fog_frequency * math::pi * 2)));
	}

	if (_last_gauntlet) 
	{
		_gauntlet_ui_go->SetActive(_last_gauntlet && !_last_gauntlet->IsCompleted());
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

	if (_audio_manager != nullptr)
	{
		_audio_manager->Restart();
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