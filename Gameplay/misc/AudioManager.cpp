#include "scriptingUtil/gameplaypch.h"
#include "AudioManager.h"

#include "modules/ModuleSceneManager.h"

#include "constants/Sounds.h"

#include "entities/enemies/EnemyController.h"

static const unsigned LEVEL_1 = 1;
static const unsigned LEVEL_2 = 2;
static const unsigned BOSS_LEVEL = 3;
static constexpr const wchar_t* GAMEPLAY_SWITCH = L"Gameplay_Switch";
static constexpr const wchar_t* GAMEPLAY_EXPLORE = L"Explore";
static constexpr const wchar_t* GAMEPLAY_COMBAT = L"Combat";

Hachiko::Scripting::AudioManager::AudioManager(GameObject* game_object)
	: Script(game_object, "AudioManager")
	, _enemies_in_combat(0)
	, _in_combat(false)
	, _in_gaunlet(false)
{
}

void Hachiko::Scripting::AudioManager::OnAwake()
{
	_audio_source = game_object->GetComponent<ComponentAudioSource>();
	SetNavigation();
	SetFootstepEffect();
}

void Hachiko::Scripting::AudioManager::OnStart()
{
	SetNavigation();
	SetFootstepEffect();
	_audio_source->SetSwitch(Sounds::SWITCH_LEVELS, GetLevelSwitchName(_level));
	_audio_source->PostEvent(Sounds::PLAY_BACKGROUND_MUSIC);
	if (_level != BOSS_LEVEL)
	{
		_audio_source->PostEvent(Sounds::PLAY_WIND);
		_audio_source->PostEvent(Sounds::PLAY_PEBBLE);
	}

	updated = true;
}

void Hachiko::Scripting::AudioManager::OnUpdate()
{
	if (updated)
	{
		return;
	}

	if (_in_gaunlet)
	{
		SetCombat();
	}
	else
	{
		SetNavigation();
	}

	updated = true;
}

void Hachiko::Scripting::AudioManager::RegisterCombat()
{
	++_enemies_in_combat;
	if (!_in_combat)
	{
		_in_combat = true;
		updated = false;
	}
}

void Hachiko::Scripting::AudioManager::UnregisterCombat()
{
	--_enemies_in_combat;
	if (_enemies_in_combat <= 0)
	{
		_enemies_in_combat = 0;
		updated = false;
		_in_combat = false;
	}
}

void Hachiko::Scripting::AudioManager::RegisterGaunlet()
{
	_in_gaunlet = true;
	_in_combat = true;
	updated = false;
}

void Hachiko::Scripting::AudioManager::UnregisterGaunlet()
{
	_in_gaunlet = false;
	_in_combat = false;
	updated = false;
}

void Hachiko::Scripting::AudioManager::PlayGaunletComplete()
{
	if (_level != BOSS_LEVEL)
	{
		_audio_source->PostEvent(Sounds::GAUNTLET_COMPLETE);
	}
}

void Hachiko::Scripting::AudioManager::PlayGaunletStart()
{
}

void Hachiko::Scripting::AudioManager::PlayGaunletNextRound()
{
	_audio_source->PostEvent(Sounds::GAUNTLET_NEXT_ROUND);
}

void Hachiko::Scripting::AudioManager::Restart()
{
	OnStart();
}

void Hachiko::Scripting::AudioManager::SetLevel(unsigned level)
{
	HE_LOG("AudioManager - Setting Level: %ud", level);
	_level = level;
}

void Hachiko::Scripting::AudioManager::PlaySpawnWorm()
{
	_audio_source->PostEvent(Sounds::WORM_ROAR);
	_audio_source->PostEvent(Sounds::WORM_SPAWN);
}

void Hachiko::Scripting::AudioManager::PlayDoorOpening()
{
	_audio_source->PostEvent(Sounds::PLAY_DOOR_OPENING);
}

void Hachiko::Scripting::AudioManager::StopBackgroundMusic()
{
	_audio_source->PostEvent(Sounds::STOP_BACKGROUND_MUSIC);
}

void Hachiko::Scripting::AudioManager::SetCombat()
{
	_audio_source->SetSwitch(GAMEPLAY_SWITCH, GAMEPLAY_COMBAT);
}

void Hachiko::Scripting::AudioManager::SetNavigation()
{
	_audio_source->SetSwitch(GAMEPLAY_SWITCH, GAMEPLAY_EXPLORE);
}

void Hachiko::Scripting::AudioManager::SetFootstepEffect()
{
	if (_level == LEVEL_1)
	{
		_audio_source->SetSwitch(Sounds::SWITCH_GROUP_FOOTSTEPS, Sounds::SWITCH_STATE_FOOTSTEPS_GRAVEL);
	}
	else
	{
		_audio_source->SetSwitch(Sounds::SWITCH_GROUP_FOOTSTEPS, Sounds::SWITCH_STATE_FOOTSTEPS_STANDARD);
	}
}

const wchar_t* Hachiko::Scripting::AudioManager::GetLevelSwitchName(unsigned level)
{
	switch (level)
	{
	case LEVEL_1:
		return Sounds::SWITCH_LEVEL_1;

	case LEVEL_2:
		return Sounds::SWITCH_LEVEL_2;

	case BOSS_LEVEL:
		return Sounds::SWITCH_LEVEL_BOSS;
	}
	return nullptr;
}

void Hachiko::Scripting::AudioManager::StopMusic()
{
	_audio_source->PostEvent(Sounds::STOP_BACKGROUND_MUSIC);
}

void Hachiko::Scripting::AudioManager::PlayEnemyDeath(EnemyType enemy_type)
{
	switch (enemy_type)
	{
	case Hachiko::Scripting::EnemyType::BEETLE:
		_audio_source->PostEvent(Sounds::BEETLE_DEATH);
		break;
	case Hachiko::Scripting::EnemyType::WORM:
		_audio_source->PostEvent(Sounds::WORM_DEATH);
		break;
	default:
		break;
	}

}
