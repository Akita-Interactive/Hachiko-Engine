#include "scriptingUtil/gameplaypch.h"
#include "AudioManager.h"

#include "modules/ModuleSceneManager.h"

#include "constants/Sounds.h"

#include "entities/enemies/EnemyController.h"

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
}

void Hachiko::Scripting::AudioManager::OnStart()
{
	SetNavigation();
	_audio_source->PostEvent(GetPlayMusicEventName(_level));
	_audio_source->PostEvent(Sounds::PLAY_WIND);
	_audio_source->PostEvent(Sounds::PLAY_PEBBLE);
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

void Hachiko::Scripting::AudioManager::Restart()
{
	OnStart();
}

void Hachiko::Scripting::AudioManager::SetLevel(unsigned level)
{
	_level = level;
}

void Hachiko::Scripting::AudioManager::SetCombat()
{
	_audio_source->SetRTPCValue(Sounds::ENEMY_AWARE, 100);
}

void Hachiko::Scripting::AudioManager::SetNavigation()
{
	_audio_source->SetRTPCValue(Sounds::ENEMY_AWARE, 0);
}

const wchar_t* Hachiko::Scripting::AudioManager::GetPlayMusicEventName(unsigned level)
{
	switch (level)
	{
	case 1:
		return Sounds::PLAY_BACKGROUND_MUSIC_LVL1;

	case 2:
		return Sounds::PLAY_BACKGROUND_MUSIC_LVL2;

	case 3:
		return Sounds::PLAY_BACKGROUND_MUSIC_BOSS;

	default:
		return Sounds::PLAY_BACKGROUND_MUSIC_LVL1;
	}
}

const wchar_t* Hachiko::Scripting::AudioManager::GetStopMusicEventName(unsigned level)
{
	switch (level)
	{
	case 1:
		return Sounds::STOP_BACKGROUND_MUSIC_LVL1;

	case 2:
		return Sounds::STOP_BACKGROUND_MUSIC_LVL2;

	case 3:
		return Sounds::STOP_BACKGROUND_MUSIC_BOSS;

	default:
		return Sounds::STOP_BACKGROUND_MUSIC_LVL1;
	}
}

void Hachiko::Scripting::AudioManager::StopMusic()
{
	_audio_source->PostEvent(GetStopMusicEventName(_level));
}
