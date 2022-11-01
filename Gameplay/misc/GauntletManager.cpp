#include "scriptingUtil/gameplaypch.h"

#include "GauntletManager.h"

#include "constants/Sounds.h"
#include "misc/LevelManager.h"
#include "misc/AudioManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "constants/Scenes.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>
#include "entities/player/CombatManager.h"

Hachiko::Scripting::GauntletManager::GauntletManager(GameObject* game_object)
	: Script(game_object, "GauntletManager")
	, _trigger_radius(5.f)
	, _round_wait_time(3.f)
	, _complete_wait_time(2.f)
	, _door_controller_1(nullptr)
	, _door_controller_2(nullptr)
	, _pack_1(nullptr)
	, _pack_2(nullptr)
	, _pack_3(nullptr)
	, _camera_anchor(nullptr)
	, _relative_position(float3::zero)
	, _camera_follows_player(0.4f)
{}

void Hachiko::Scripting::GauntletManager::OnAwake()
{
	_combat_manager = _combat_manager_go->GetComponent<CombatManager>();
	_doors.clear();
	if (_door_controller_1 && _door_controller_1->GetComponent<DoorController>()) _doors.push_back(_door_controller_1->GetComponent<DoorController>());
	if (_door_controller_2 && _door_controller_2->GetComponent<DoorController>()) _doors.push_back(_door_controller_2->GetComponent<DoorController>());
	
	_enemy_packs.clear();
	if (_pack_1) _enemy_packs.push_back(_pack_1);
	if (_pack_2) _enemy_packs.push_back(_pack_2);
	if (_pack_3) _enemy_packs.push_back(_pack_3);
	for (GameObject* _pack : _enemy_packs)
	{
		for (GameObject* enemy : _pack->children)
		{
			ComponentAgent* agc = enemy->GetComponent<ComponentAgent>();
			if (agc)
			{
				agc->RemoveFromCrowd();
			}
		}
		_pack->SetActive(false);
	}

	_audio_manager = Scenes::GetAudioManager()->GetComponent<AudioManager>();
}

void Hachiko::Scripting::GauntletManager::OnStart()
{
	_level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
	_main_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
	game_object->SetVisible(false, false);

	ResetGauntlet(true);
}

void Hachiko::Scripting::GauntletManager::OnUpdate()
{
	if (completed)
	{
		if (_closing_door && _closing_door->IsActive())
		{
			_elapsed_time += Time::DeltaTimeScaled();
			if (_elapsed_time < 2.0f)
			{
				_closing_door->ChangeDissolveProgress(1 - (_elapsed_time / 2.0f), true);
			}
			else
			{
				_closing_door->SetActive(false);
			}
		}
		
		return;
	}
	
	if (!started)
	{
		if (_trigger_radius >= game_object->GetTransform()->GetGlobalPosition().Distance(_combat_manager->GetPlayer()->GetTransform()->GetGlobalPosition()))
		{
			// Here we would start spawning the first round
			StartGauntlet();
		}
	}
	else
	{
		if (_closing_door && _elapsed_time < 2.0f)
		{
			_elapsed_time += Time::DeltaTimeScaled();
			_closing_door->ChangeDissolveProgress(_elapsed_time, true);
		}
		else
		{
			ControllCameraPos();
			CheckRoundStatus();
		}
	}

}

void Hachiko::Scripting::GauntletManager::ResetGauntlet(bool complete_reset)
{
	CloseDoors();
	if (_closing_door) _closing_door->SetActive(false);
	for (GameObject* enemy_pack : _enemy_packs)
	{
		_combat_manager->DeactivateEnemyPack(enemy_pack);
	}
	started = false;
	
	remaining_between_round_time = 0.f;

	if (!complete_reset)
	{
		return;
	}
	current_round = 0;
}

void Hachiko::Scripting::GauntletManager::StartGauntlet()
{
	started = true;
	SpawnRound(current_round);

	// Notify level manager
	_level_manager->SetGauntlet(this);

	// Notify audio manager
	_audio_manager->RegisterGaunlet();
	_audio_manager->PlayGaunletStart();

	// Set the camera to its position if there is an anchor set
	if (_camera_anchor && _central_anchor)
	{
		_main_camera->ChangeRelativePosition(_relative_position, false, .2f, 0.0f);
		_main_camera->SetObjective(_camera_anchor);
	}

	if (_closing_door)
	{
		_closing_door->SetActive(true);
		_closing_door->ChangeDissolveProgress(0.0f, true);
	}
}

bool Hachiko::Scripting::GauntletManager::IsFinished() const
{
	return current_round >= _enemy_packs.size();
}

void Hachiko::Scripting::GauntletManager::CheckRoundStatus()
{
	if (current_round >= _enemy_packs.size()) {
		completed = true;
		_elapsed_time = 0.0f;
		OpenDoors();
		_audio_manager->UnregisterGaunlet();
		_main_camera->ChangeRelativePosition(Scenes::GetPlayer()->GetComponent<PlayerController>()->GetCamBasicPos(), false, .2f, 0.0f);
		_main_camera->SetObjective(Scenes::GetPlayer());
		return;
	}

	unsigned alive_count = _combat_manager->GetPackAliveCount(_enemy_packs[current_round]);

	_level_manager->SetEnemyCount(alive_count);

	if(alive_count > 0) return;

	if (!changing_rounds)
	{
		changing_rounds = true;
		
		// If it is the last round end instantly
		if (IsFinished())
		{
			remaining_between_round_time = _complete_wait_time;
			return;
		}
		else
		{
			remaining_between_round_time = _round_wait_time;
		}
	}

	remaining_between_round_time -= Time::DeltaTime();

	if (remaining_between_round_time <= 0.f)
	{
		changing_rounds = false;
		++current_round;
		if (IsFinished())
		{
			_audio_manager->PlayGaunletComplete();
			return;
		}
		_audio_manager->PlayGaunletNextRound();
		SpawnRound(current_round);
	}	
}

void Hachiko::Scripting::GauntletManager::OpenDoors()
{
	for (DoorController* door : _doors)
	{
		door->Open();
	}
}

void Hachiko::Scripting::GauntletManager::CloseDoors()
{
	for (DoorController* door : _doors)
	{
		door->Close();
	}
}

void Hachiko::Scripting::GauntletManager::SpawnRound(unsigned round)
{
	if (round >= _enemy_packs.size()) return;
	_combat_manager->ActivateEnemyPack(_enemy_packs[round]);
	_combat_manager->ResetEnemyPack(_enemy_packs[round], true);
}

void Hachiko::Scripting::GauntletManager::ControllCameraPos()
{
	if (_camera_anchor && _central_anchor)
	{
		float3 camera_a_pos = _camera_anchor->GetTransform()->GetGlobalPosition();
		float3 player_pos = Scenes::GetPlayer()->GetTransform()->GetGlobalPosition();
		float3 center_pos = _central_anchor->GetTransform()->GetGlobalPosition();
		camera_a_pos = math::Lerp(center_pos, player_pos, _camera_follows_player);
		_camera_anchor->GetTransform()->SetGlobalPosition(camera_a_pos);
	}
}