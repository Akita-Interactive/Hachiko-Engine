#include "scriptingUtil/gameplaypch.h"

#include "GauntletManager.h"

#include "misc/LevelManager.h"
#include "constants/Scenes.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

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
}

void Hachiko::Scripting::GauntletManager::OnStart()
{
	game_object->SetVisible(false, false);

	ResetGauntlet();
}

void Hachiko::Scripting::GauntletManager::OnUpdate()
{
	if (completed) return;
	
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
		CheckRoundStatus();
	}

}

void Hachiko::Scripting::GauntletManager::StartGauntlet()
{
	started = true;
	current_round = 0;
	SpawnRound(current_round);

	// Notify level manager
	LevelManager* level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
	level_manager->SetLastGauntlet(this);
}

void Hachiko::Scripting::GauntletManager::ResetGauntlet()
{
	CloseDoors();
	for (GameObject* enemy_pack : _enemy_packs)
	{
		_combat_manager->DeactivateEnemyPack(enemy_pack);
	}
	started = false;
	current_round = 0;
	remaining_between_round_time = 0.f;
}

bool Hachiko::Scripting::GauntletManager::IsFinished() const
{
	return current_round >= _enemy_packs.size();
}

void Hachiko::Scripting::GauntletManager::CheckRoundStatus()
{
	if (current_round >= _enemy_packs.size()) {
		completed = true;
		OpenDoors();
		return;
	}

	if(!_combat_manager->IsPackDead(_enemy_packs[current_round])) return;

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
			return;
		}
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
	_combat_manager->ResetEnemyPack(_enemy_packs[round], true);
	_combat_manager->ActivateEnemyPack(_enemy_packs[round]);
}
